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

#ifndef OPENAWE_MOUSECONTROLLEDFREECAMERA_H
#define OPENAWE_MOUSECONTROLLEDFREECAMERA_H

#include "src/events/event.h"

#include "src/graphics/freecamera.h"

enum InvertControls {
	kDoNotInvert = 1,
	kDoInvert = -1
};

class MouseControlledFreeCamera : public Graphics::FreeCamera {
public:
	MouseControlledFreeCamera();
	void update(float delta) override;

private:
	void handleEvent(const Events::Event &event);

protected:
	float _mouseSensitivity;
	double _yaw, _pitch, _roll;
	InvertControls _invertY;
};


#endif //OPENAWE_MOUSECONTROLLEDFREECAMERA_H
