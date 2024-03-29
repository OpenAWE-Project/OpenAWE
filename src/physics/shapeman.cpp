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

#include "src/physics/shapeman.h"
#include "src/physics/havokshape.h"

namespace Physics {

ShapePtr ShapeManager::get(rid_t rid) {
    const auto iter = _shapes.find(rid);
    if (iter != _shapes.end())
        return iter->second;

    try {
        const auto shape = std::make_shared<HavokShape>(rid);
        _shapes[rid] = shape;
        return shape;
    } catch (std::exception &e) {
        spdlog::error("Failed to create Physics shape: {}", e.what());
        _shapes[rid] = ShapePtr();
        return {};
    }
}

} // Physics