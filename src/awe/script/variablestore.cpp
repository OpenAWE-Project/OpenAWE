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

#include "src/common/exception.h"

#include "src/awe/script/variablestore.h"

namespace AWE::Script {

VariableStore::VariableStore(const std::vector <Variable> &variables, const DebugEntries &entries) :
	_variables(variables),
	_debugEntries(entries) {
}

std::string VariableStore::getDebugName(byte id) const {
	auto iter = _debugEntries.find(id);
	if (iter != _debugEntries.end())
		return iter->second;
	return "";
}

void VariableStore::setVariable(byte id, Variable variable) {
	if (id > _variables.size())
		throw Common::Exception("Invalid variable id");
	_variables[id] = variable;
}

Variable VariableStore::getVariable(byte id) const {
	if (id > _variables.size())
		throw Common::Exception("Invalid variable id");
	return _variables[id];
}

} // End of namespace AWE::Script
