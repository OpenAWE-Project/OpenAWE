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
#include "src/awe/dpfile.h"

namespace AWE::Script {

class Functions {
public:
	Functions(entt::registry &registry, entt::scheduler<double> &scheduler);

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
			std::vector<Variable> parameters,
			const std::shared_ptr<DPFile> &dp
	);

	/*!
	 * Call a global object method
	 *
	 * \param name
	 * \param functionName
	 * \param parameters
	 * \return
	 */
	std::optional<Variable> callGlobal(
			const std::string &name,
			const std::string &functionName,
			std::vector<Variable> parameters,
			const std::shared_ptr<DPFile> &dp
	);

	void setTime(float time);

protected:
	struct Context {
		entt::entity thisEntity;
		std::shared_ptr<DPFile> dp;
		std::vector<Variable> parameters;
		std::optional<Variable> ret;

		float getFloat(size_t index) {
			return std::get<Number>(parameters[index]).floatingPoint;
		}

		int getInt(size_t index) {
			return std::get<Number>(parameters[index]).integer;
		}

		bool getBool(size_t index) {
			return std::get<Number>(parameters[index]).integer != 0;
		}

		entt::entity getEntity(size_t index) {
			return std::get<entt::entity>(parameters[index]);
		}

		std::string getString(size_t index) {
			return dp->getString(std::get<Number>(parameters[index]).integer);
		}
	};

	enum ParameterType {
		kInt,
		kFloat,
		kBool,
		kString,
		kEntity
	};

	template<typename F>
	struct NativeFunction {
		std::function<void(F *, class Context &)> func;
		std::vector<ParameterType> signature;
	};

	std::string getFunctionString(
			const std::string &name,
			const std::vector<ParameterType> &signature,
			const std::vector<Variable> &parameters,
			const std::shared_ptr<DPFile> &dp
	) const;

	virtual void callFunction(const std::string &name, Context &ctx);

	entt::registry &_registry;
	entt::scheduler<double> &_scheduler;
	float _time;

private:
	// functions_object.cpp
	void sendCustomEvent(Context &ctx);

    // functions_game.cpp
    void getRand01(Context &ctx);
    void getRand(Context &ctx);
    void getRandInt(Context &ctx);

	static const std::map<std::string, NativeFunction<Functions>> _functions;
};

} // End of namespace AWE::Script

#endif //OPENAWE_FUNCTIONS_H
