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
	Functions(entt::registry &registry, Engine &engine) : AWE::Script::Functions(registry), _engine(engine) {
	}

	Engine &getEngine();

protected:
	NativeFunction getFunction(const std::string &name) override;

private:
	// functions_game.cpp
	static void activateTask(Context &ctx);
	static void playMusic(Context &ctx);
	static void getStoryModeRound(Context &ctx);
	static void isTrial(Context &ctx);
	static void isTrialModeB(Context &ctx);
	static void isManuscriptPageUnlocked(Context &ctx);

	// functions_object.cpp
	static void hide(Context &ctx);

	// functions_keyframe.cpp
	static void animate(Context &ctx);

	// Functions_timer.cpp
	static void startTimer(Context &ctx);
	static void startTimerWithDuration(Context &ctx);

	static const std::map<std::string, NativeFunction> _functions;

	Engine &_engine;
};

}

#endif //OPENAWE_FUNCTIONS_H
