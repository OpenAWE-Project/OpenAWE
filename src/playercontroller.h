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

#ifndef OPENAWE_PLAYERCONTROLLER_H
#define OPENAWE_PLAYERCONTROLLER_H

#include <optional>

#include <entt/entt.hpp>

#include "src/graphics/animationcontroller.h"

#include "src/events/eventman.h"
#include "src/physics/charactercontroller.h"
#include "src/transform.h"

class PlayerController {
public:
	PlayerController(entt::registry &registry);

	void setPlayerCharacter(entt::entity pc);

	void update(float time);

private:
	void handleEvent(const Events::Event &event);

	float _time;
	std::optional<entt::entity> _pc;
    glm::vec3 _baseDirection;
	entt::registry &_registry;
	TransformPtr _transform;
	Graphics::AnimationControllerPtr _animController;
	Physics::CharacterControllerPtr _charController;
};


#endif //OPENAWE_PLAYERCONTROLLER_H
