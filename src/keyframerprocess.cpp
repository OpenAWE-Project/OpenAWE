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

#include "src/keyframerprocess.h"
#include "src/transform.h"

KeyFramerProcess::KeyFramerProcess(entt::entity keyFramerEntity, entt::registry &registry) :
	_keyFramerEntity(keyFramerEntity), _registry(registry) {
	_keyFramer = _registry.get<KeyFramerPtr>(_keyFramerEntity);
}

void KeyFramerProcess::update(float delta, void *) {
	if (!_keyFramer->hasAnimation()) {
		succeed();
		return;
	}

	_keyFramer->update(delta);

	for (const auto &affectedEntity: _keyFramer->getAffectedEntities()) {
		_registry.patch<Transform>(affectedEntity, [&](auto &transform){
			transform.setKeyFramerTransform(_keyFramer->getTransformation(), _keyFramer->isAbsolute());
		});
	}
}
