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

#ifndef OPENAWE_VARIABLESTORE_H
#define OPENAWE_VARIABLESTORE_H

#include <vector>

#include "src/common/types.h"

#include "src/awe/script/bytecode.h"
#include "src/awe/script/types.h"

namespace AWE::Script {

class VariableStore;

typedef std::shared_ptr<VariableStore> VariableStorePtr;

/*!
 * \brief Class storing bytecode variables
 *
 *
 */
class VariableStore {
public:
	VariableStore(const std::vector<Variable> &variables, const DebugEntries &entries);

	/*!
	 * Get the debug name for the specified variable
	 * @param id the variable to search fro
	 * @return The debug name if available or an empty string
	 */
	std::string getDebugName(byte id) const;

	/*!
	 * Set variable specified by id for this bytecode
	 * \param id The id of the variable
	 * \param variable The variable to set
	 */
	void setVariable(byte id, Variable variable);

	/*!
	 * Get a variable from this bytecode specified by id
	 * \param id The id of the variable
	 * \return The requested variable
	 */
	Variable getVariable(byte id) const;

private:
	std::vector<Variable> _variables;
	const DebugEntries _debugEntries;
};

} // End of namespace AWE::Script

#endif //OPENAWE_VARIABLESTORE_H
