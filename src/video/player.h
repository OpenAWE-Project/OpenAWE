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

#ifndef AWE_PLAYER_H
#define AWE_PLAYER_H

#include <memory>
#include <future>
#include <queue>
#include <chrono>
#include <vector>
#include <atomic>

#include "src/common/uuid.h"

#include "src/graphics/texture.h"
#include "src/graphics/images/surface.h"

#include "src/sound/stream.h"

#include "src/codecs/oggcontainer.h"

namespace Video {

class Player {
public:
	Player();
	~Player();

	void setAudioTracks(std::initializer_list<unsigned int> ids);

	Graphics::TexturePtr getTexture();

	void load(const std::string &videoFile);
	void play();
	void stop();
	bool isPlaying() const;

	void update();

private:
	void prepareTextures();
	void prepareSurfaces();

	void preloadLoop();

	const bool _ssse3;

	std::chrono::milliseconds _frameDuration;
	std::chrono::time_point<std::chrono::system_clock> _lastFrame;

	std::atomic_bool _playing;

	/** Variable to wait until the preloading step is successfully stopped */
	std::mutex _stopped;

	Codecs::YCbCrBuffer _ycbcr;

	Graphics::ProxyTexturePtr _proxyTexture;
	Graphics::TexturePtr _currentFrame;
	std::deque<Graphics::SurfacePtr> _availableSurfaces;
	std::deque<Graphics::TexturePtr> _availableTextures;
	std::deque<Graphics::SurfacePtr> _preparedSurfaces;
	std::deque<Graphics::TexturePtr> _preparedTextures;
	std::vector<std::unique_ptr<Sound::Stream>> _streams;
	std::unique_ptr<Codecs::Container> _container;
	std::unique_ptr<Codecs::VideoStream> _video;
};

}

#endif //AWE_PLAYER_H
