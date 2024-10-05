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

#ifndef OPENAWE_AWAN_FUNCTIONS_H
#define OPENAWE_AWAN_FUNCTIONS_H

#include <entt/entt.hpp>

#include "src/awe/script/functions.h"

#include "src/engines/awan/engine.h"

namespace Engines::AlanWakesAmericanNightmare {

class Functions : public AWE::Script::Functions {
public:
	Functions(entt::registry &registry, Engine &engine) : AWE::Script::Functions(registry, engine.getScheduler()), _engine(engine) {
	}

	Engine &getEngine();

protected:
	void callFunction(const std::string &name, Context &ctx) override;

private:
	// functions_game.cpp
	void activateTask(Context &ctx);
	void playMusic(Context &ctx);
	void getStoryModeRound(Context &ctx);
	void isTrial(Context &ctx);
	void isTrialModeB(Context &ctx);
	void isManuscriptPageUnlocked(Context &ctx);
	void setSunYRotation(Context &ctx);
	void setTime(Context &ctx);

	// functions_object.cpp
	void hide(Context &ctx);

	// functions_keyframe.cpp
	void animate(Context &ctx);

	// Functions_timer.cpp
	void startTimer(Context &ctx);
	void startTimerWithDuration(Context &ctx);

	// functions_light.cpp
	void enablePointLight(Context &ctx);

	// functions_ai.cpp
	void aiAddAnimate(Context &ctx);
	void aiAddAnimateLooping(Context &ctx);

	static const std::map<std::string, NativeFunction<Functions>> _functions;

	Engine &_engine;
};

}

#endif //OPENAWE_FUNCTIONS_H
