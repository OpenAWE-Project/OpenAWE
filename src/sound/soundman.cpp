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

SoundManager::SoundManager() : _device(nullptr), _context(nullptr) {
	//int ret = alcIsExtensionPresent(_device, "ALC_ENUMERATION_EXT");
	//assert(alcIsExtensionPresent(_device, "AL_EXT_FLOAT32") == AL_TRUE);
}

SoundManager::~SoundManager() {
	alDeleteSources(_sourcePool.size(), _sourcePool.data());
	alDeleteBuffers(_bufferPool.size(), _bufferPool.data());

	alcMakeContextCurrent(nullptr);
	alcDestroyContext(_context);
	alcCloseDevice(_device);
}

void SoundManager::init() {
	_device = alcOpenDevice(nullptr);
	_context = alcCreateContext(_device, nullptr);

	alcMakeContextCurrent(_context);

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

}