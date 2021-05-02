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

#ifndef OPENAWE_FUNCTIONS_H
#define OPENAWE_FUNCTIONS_H

#include <functional>
#include <map>
#include <optional>
#include <random>

#include "types.h"

namespace AWE::Script {

class Functions {
public:
	Functions();

	/*!
	 * Call an object with a certain function and optionally return a value
	 *
	 * \param object the object to call the function for
	 * \param functionName
	 * \param parameters
	 * \return
	 */
	std::optional<Variable> callObject(
			entt::entity object,
			const std::string &functionName,
			std::vector<Variable> parameters
	);

	/*!
	 * Call a global object method
	 *
	 * \param className
	 * \param functionName
	 * \param parameters
	 * \return
	 */
	std::optional<Variable> callGlobal(
			const std::string &className,
			const std::string &functionName,
			std::vector<Variable> parameters
	);

protected:
	struct Context {
		entt::entity thisEntity;
		Functions &functions;
		std::vector<Variable> parameters;
		std::optional<Variable> ret;

		float getFloat(size_t index) {
			return reinterpret_cast<float&>(std::get<uint32_t>(parameters[index]));
		}

		int getInt(size_t index) {
			return reinterpret_cast<int32_t&>(std::get<uint32_t>(parameters[index]));
		}

		entt::entity getEntity(size_t index) {
			return std::get<entt::entity>(parameters[index]);
		}

		template<typename T> T& getFunctions() {
			return reinterpret_cast<T&>(functions);
		}
	};

	typedef std::function<void(Context &)> NativeFunction;

	virtual NativeFunction getFunction(const std::string &name);


private:
    // functions_game.cpp
    static void getRand01(Context &ctx);
    static void getRand(Context &ctx);
    static void getRandInt(Context &ctx);

	static std::map<std::string, NativeFunction> _functions;
};

} // End of namespace AWE::Script

#endif //OPENAWE_FUNCTIONS_H
