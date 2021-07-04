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

#ifndef OPENAWE_DEBUGDRAW_H
#define OPENAWE_DEBUGDRAW_H

#include "src/graphics/linelist.h"

#include <btBulletDynamicsCommon.h>

namespace Physics {

/*!
 * \brief helper class for visualizing physics
 *
 * This class uses a line list to draw bullet physics data into the screen,
 * to help debugging the physics system
 */
class DebugDraw : public btIDebugDraw {
public:
	void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color) override;
	void drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime,
						  const btVector3 &color) override;
	void reportErrorWarning(const char *warningString) override;
	void draw3dText(const btVector3 &location, const char *textString) override;
	void setDebugMode(int debugMode) override;
	int getDebugMode() const override;

	void clearLines() override;
	void flushLines() override;

private:
	Graphics::LineList _lineList;
	int _debugMode;
};

}

#endif //OPENAWE_DEBUGDRAW_H
