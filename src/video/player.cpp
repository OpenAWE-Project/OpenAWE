/* OpenAWE - A reimplementation of Remedys Alan Wake Engine
 *
 * OpenAWE is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * OpenAWE is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * OpenAWE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenAWE. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <assert.h>
#include <memory>

#include <spdlog/spdlog.h>

#include "src/common/cpuinfo.h"
#include "src/common/threadpool.h"

#include "src/awe/resman.h"

#include "src/codecs/theora.h"
#include "src/codecs/yuv2rgb.h"

#include "src/graphics/gfxman.h"
#include "src/graphics/images/surface.h"

#include "src/video/player.h"

namespace Video {

Player::Player() :
		_ssse3(Common::hasSSSE3()),
		_playing(false),
		_proxyTexture(GfxMan.createProxyTexture()) {

}

Player::~Player() {
	if (_playing)
		stop();
}

void Player::setAudioTracks(std::initializer_list<unsigned int> ids) {
	for (const auto &id: ids) {
		if (!_container)
			throw CreateException("Trying t add audio track to not loaded container");

		if (_container->getNumAudioStreams() <= id) {
			spdlog::warn(
				"Invalid audio stream id {} for video with {} audio streams",
				id,
				_container->getNumAudioStreams()
			);
			continue;
		}

		_streams.emplace_back(std::make_unique<Sound::Stream>(_container->createAudioStream(id)));
	}
}

Graphics::TexturePtr Player::getTexture() {
	return _proxyTexture;
}

void Player::load(const std::string &videoFile) {
	_container = std::make_unique<Codecs::OggContainer>(ResMan.getResource(videoFile));

	if (!_container->hasVideoStream())
		throw CreateException("Trying to load container file without a video stream");

	_video.reset(_container->createVideoStream());

	// Generate new textures for the specific video
	_availableTextures.clear();
	for (int i = 0; i < 64; ++i) {
		_availableTextures.emplace_back(GfxMan.createEmptyTexture2D(
			kRGB8,
			_video->getWidth(),
			_video->getHeight(),
			std::format("Video Surface {}/64", i)
		));
	}

	_availableSurfaces.clear();
	for (int i = 0; i < 64; ++i) {
		_availableSurfaces.push_back(std::make_shared<Graphics::Surface>(
			_video->getWidth(),
			_video->getHeight(),
			kRGB8
		));
	}

	_video->allocateBuffer(_ycbcr);

	_frameDuration = std::chrono::milliseconds(static_cast<long>(1000 / _video->getFps()));

	prepareSurfaces();
	prepareTextures();

	// Set first frame
	_currentFrame = _preparedTextures.front();
	_preparedTextures.pop_front();
	_proxyTexture->assign(_currentFrame);
}

void Player::play() {
	if (!_container)
		throw CreateException("Tried to play a video, which is not loaded");

	_playing = true;
	_lastFrame = std::chrono::system_clock::now();

	for (auto &stream: _streams) {
		stream->play();
	}

	Threads.add([this](){ preloadLoop(); });
}

void Player::stop() {
	_stopped.lock();
	_playing = false;
	for (auto &stream: _streams) {
		stream->stop();
	}

	_stopped.lock();
	_stopped.unlock();
}

bool Player::isPlaying() const {
	return _playing;
}

void Player::update() {
	if (!_playing)
		return;

	const auto now = std::chrono::system_clock::now();
	const auto currentDuration = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastFrame);
	if (currentDuration >= _frameDuration) {
		const unsigned int framesProgressed = currentDuration.count() / _frameDuration.count();

		unsigned int framesDeleted = 0;
		do {
			prepareTextures();

			for (unsigned int i = 0; i < std::min<unsigned int>(std::max(framesProgressed - 1 - framesDeleted, 0u), _preparedTextures.size()); ++i) {
				_availableTextures.emplace_back(_preparedTextures.front());
				_preparedTextures.pop_front();
				framesDeleted++;
			}
		} while (_preparedTextures.empty());

		if (_preparedTextures.empty()) {
			spdlog::warn("Video decoding too slow, dropping frame");
			return;
		}

		if (_currentFrame)
			_availableTextures.push_back(_currentFrame);

		_lastFrame = now;
		_currentFrame = _preparedTextures.front();
		_proxyTexture->assign(_currentFrame);
		_preparedTextures.pop_front();
	}

	if (_preparedTextures.empty() && _video->eos())
		_playing = false;
}

void Player::prepareTextures() {
	while (!_availableTextures.empty() && !_preparedSurfaces.empty() && _preparedTextures.size() < 32) {
		auto frameTexture = _availableTextures.front();
		_availableTextures.pop_front();
		std::shared_ptr<Graphics::Surface> frameSurface = _preparedSurfaces.front();
		_preparedSurfaces.pop_front();

		frameTexture->load(std::move(*frameSurface));
		_preparedTextures.push_back(frameTexture);
		_availableSurfaces.push_back(frameSurface);
	}
}

void Player::prepareSurfaces() {
	while (!_availableSurfaces.empty() && !_video->eos()) {
		_video->readNextFrame(_ycbcr);
		auto surface = _availableSurfaces.front();
		_availableSurfaces.pop_front();
		if (_ssse3) {
			Codecs::convertYUV2RGB_SSSE3(
					_ycbcr,
					surface->getData<std::byte>(),
					_video->getWidth(),
					_video->getHeight()
			);
		} else {
			Codecs::convertYUV2RGB(
				_ycbcr,
				surface->getData<std::byte>(),
				_video->getWidth(),
				_video->getHeight()
			);
		}

		_preparedSurfaces.push_back(surface);
	}
}

void Player::preloadLoop() {
	if (!_playing) {
		_stopped.unlock();
		return;
	}

	prepareSurfaces();

	Threads.add([this](){ preloadLoop(); });
}

}
