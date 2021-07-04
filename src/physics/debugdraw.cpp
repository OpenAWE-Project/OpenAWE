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

#include "debugdraw.h"

namespace Physics {

void DebugDraw::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) {
	glm::vec3 fromVec(from.x(), from.y(), from.z());
	glm::vec3 toVec(to.x(), to.y(), to.z());
	glm::vec3 colorVec(color.x(), color.y(), color.z());

	_lineList.addLine(fromVec, toVec, colorVec);
}

void DebugDraw::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime,
								 const btVector3 &color) {

}

void DebugDraw::reportErrorWarning(const char *warningString) {
	spdlog::warn("Bullet Warning: {}", warningString);
}

void DebugDraw::draw3dText(const btVector3 &location, const char *textString) {

}

void DebugDraw::setDebugMode(int debugMode) {
	_debugMode = debugMode;
}

int DebugDraw::getDebugMode() const {
	return _debugMode;
}

void DebugDraw::clearLines() {
	_lineList.clear();
}

void DebugDraw::flushLines() {
	_lineList.flush();
}

}
