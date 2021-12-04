//
// Created by patrick on 19.04.20.
//

#include <iostream>
#include <src/common/threadpool.h>
#include <src/awe/resman.h>
#include <assert.h>
#include <fmt/format.h>
#include <memory>
#include <src/video/codecs/theora.h>

#include "src/graphics/images/surface.h"
#include "src/graphics/gfxman.h"

#include "src/video/player.h"

namespace Video {

Player::Player(unsigned int bufferSize) :
	_bufferSize(bufferSize),
	_currentFrame(Common::UUID::generateNil()),
	_stop(false) {
}

void Player::addAudioTrack(unsigned int id) {
	std::lock_guard<std::mutex> g(_audioStreamsMutex);

	_audio.emplace_back();
	_audio.back() = std::make_unique<AudioStream>();
	_audio.back()->id = id;
	_audio.back()->stream = std::make_unique<Sound::Stream>();
}

void Player::load(const std::string &videoFile) {
	_codec = std::make_unique<Theora>(ResMan.getResource(videoFile));

	_frameDuration = std::chrono::milliseconds(static_cast<int>(1000.0f / _codec->getFps()));

	preloadLoop();
}

void Player::play() {
	_lastFrame = std::chrono::system_clock::now();

	Common::ThreadPool::instance().add([this] { playLoop(); });

	for (auto &audio : _audio) {
		audio->stream->play();
	}
}

void Player::playLoop() {
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	if (!_texturesToDisplay.empty() && now - _lastFrame >= _frameDuration) {
		Common::UUID lastFrame = _currentFrame;
		_currentFrame = _texturesToDisplay.front();
		GfxMan.setCurrentVideoFrame(_currentFrame);
		// TODO
		/*if (!lastFrame.isNil())
			GfxMan.deregisterTextureAsync(lastFrame);*/
		_texturesToDisplay.pop();
		_lastFrame = now;
	} else if (_stop && _texturesToProcess.empty() && _texturesToDisplay.empty()) {
		Common::UUID lastFrame = _currentFrame;
		_currentFrame = Common::UUID::generateNil();
		GfxMan.setCurrentVideoFrame(_currentFrame);
		// TODO
		/*if (!lastFrame.isNil())
			GfxMan.deregisterTextureAsync(lastFrame);*/
		return;
	}

	Common::ThreadPool::instance().add([this] { playLoop(); });
}

void Player::preloadLoop() {
	while (_texturesToProcess.size() < _bufferSize && !_stop) {
		if (_codec->eos())
			_stop = true;

		auto *frame = _codec->getNextFrame();

		/*_texturesToProcess.push({

			//GfxMan.registerTextureAsync(*frame),
			std::unique_ptr<Graphics::Surface>(frame)
		});*/
	}

	_audioStreamsMutex.lock();
	for (auto &audio : _audio) {
		for (int j = 0; j < audio->stream->getNumBuffersToUnqueue(); ++j) {
			audio->stream->unqueue(audio->buffers.front());
			audio->buffers.pop();
		}

		if (audio->stream->getNumBuffersInQueue() >= 128)
			continue;

		for (int i = 0; i < 32; ++i) {
			audio->buffers.emplace();
			_codec->getNextAudio(audio->buffers.back(), audio->id);
			audio->stream->queue(audio->buffers.back());
		}
	}
	_audioStreamsMutex.unlock();

	while (!_texturesToProcess.empty() && _texturesToDisplay.size() < _bufferSize && _texturesToProcess.front().textureFuture.wait_for(std::chrono::milliseconds(10)) == std::future_status::ready) {
		_texturesToDisplay.push(_texturesToProcess.front().textureFuture.get());
		_texturesToProcess.pop();
	}

	if (_stop && _texturesToProcess.empty())
		return;

	Common::ThreadPool::instance().add([this] { preloadLoop(); });
}

}
