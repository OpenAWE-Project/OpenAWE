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

#include <spdlog/spdlog.h>

#include <assert.h>

#include "src/codecs/audiostream.h"

#include "src/sound/stream.h"
#include "soundman.h"

namespace Sound {

SoundManager::SoundManager() : _device(nullptr), _context(nullptr), _enabled(false) {
	//int ret = alcIsExtensionPresent(_device, "ALC_ENUMERATION_EXT");
	//assert(alcIsExtensionPresent(_device, "AL_EXT_FLOAT32") == AL_TRUE);
}

SoundManager::~SoundManager() {
	if (!_enabled)
		return;

	alDeleteSources(_sourcePool.size(), _sourcePool.data());
	alDeleteBuffers(_bufferPool.size(), _bufferPool.data());

	alcMakeContextCurrent(nullptr);
	alcDestroyContext(_context);
	alcCloseDevice(_device);
}

void SoundManager::init() {
	_device = alcOpenDevice(nullptr);
	if (!_device) {
		spdlog::warn("Sound disabled: failed to open default audio device");
		return;
	}

	_context = alcCreateContext(_device, nullptr);
	if (!_context) {
		spdlog::warn("Sound disabled: failed to create OpenAL context");
		alcCloseDevice(_device);
		_device = nullptr;
		return;
	}

	if (!alcMakeContextCurrent(_context)) {
		spdlog::warn("Sound disabled: failed to activate OpenAL context");
		alcDestroyContext(_context);
		_context = nullptr;
		alcCloseDevice(_device);
		_device = nullptr;
		return;
	}

	_enabled = true;

	// On macOS, openal-soft starts its coreaudio output unit when the context is created; when
	// the unit cannot be started (headless/SSH session, no usable output device), openal-soft
	// logs "[ALSOFT] (EE) AudioOutputUnitStart failed: what (2003329396)" and returns no context.
	// 'what' is the four char code of kAudioHardwareIllegalOperationError from the CoreAudio HAL,
	// so degrade to silent mode instead of letting the failed init take the whole game down.

	ALCenum major, minor;
	alcGetIntegerv(_device, ALC_MAJOR_VERSION, 1, &major);
	alcGetIntegerv(_device, ALC_MINOR_VERSION, 1, &minor);

	ALCenum monoSources, stereoSources;
	alcGetIntegerv(_device, ALC_MONO_SOURCES, 1, &monoSources);
	alcGetIntegerv(_device, ALC_STEREO_SOURCES, 1, &stereoSources);

	spdlog::info("OpenAL Vendor: {}", alGetString(AL_VENDOR));
	spdlog::info("OpenAL Renderer: {}", alGetString(AL_RENDERER));
	spdlog::info("OpenAL Version: {}.{}", major, minor);
	spdlog::info("OpenAL Version String: {}", alGetString(AL_VERSION));
	spdlog::info("OpenAL Mono Sources: {}", monoSources);
	spdlog::info("OpenAL Stereo Sources: {}", stereoSources);
	spdlog::info("OpenAL Context Extensions: {}", alcGetString(_device, ALC_EXTENSIONS));
	spdlog::info("OpenAL Extensions: {}", alGetString(AL_EXTENSIONS));


	_sourcePool.resize(16);
	alGenSources(16, _sourcePool.data());

	_bufferPool.resize(16);
	alGenBuffers(16, _bufferPool.data());

	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
}

ALuint SoundManager::registerSource() {
	ALuint id = _sourcePool.back();
	_sourcePool.pop_back();
	return id;
}

void SoundManager::deregisterSource(ALuint id) {
	_sourcePool.push_back(id);
}

ALuint SoundManager::registerBuffer() {
	if (_bufferPool.empty()) {
		_bufferPool.resize(16);
		alGenBuffers(16, _bufferPool.data());
	}

	ALuint id = _bufferPool.back();
	_bufferPool.pop_back();
	return id;
}

void SoundManager::deregisterBuffer(ALuint id) {
	_bufferPool.push_back(id);
}

bool SoundManager::isEnabled() const {
	return _enabled;
}

}