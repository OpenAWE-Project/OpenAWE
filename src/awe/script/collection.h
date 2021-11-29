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

#ifndef AWE_COLLECTION_H
#define AWE_COLLECTION_H

#include <memory>

#include "src/awe/cidfile.h"
#include "src/awe/dpfile.h"
#include "src/awe/object.h"
#include "src/awe/script/bytecode.h"
#include "src/awe/script/variablestore.h"
#include "src/awe/script/disassembler.h"

namespace AWE::Script {

/*!
 * \brief A collection of scripts
 *
 * A collection of scripts associated with an episode,
 * a level, a global world scope or the global scope
 */
class Collection {
public:
	Collection(Common::ReadStream *bytecode, Common::ReadStream *bytecodeParameters);

	/*!
	 *
	 * @param script
	 * @param bytecode
	 * @param variablesStore
	 */
	void createScript(
		const AWE::Templates::ScriptVariables &script,
		BytecodePtr &bytecode,
		VariableStorePtr &variablesStore
	);

	/*!
	 * Create a disassembler from the given bytecode and parameters
	 * \param script The script object to use for generation of the bytecode
	 * \return A newly created disassembler
	 */
	Disassembler createDisassembler(const AWE::Templates::ScriptVariables &script);

private:
	std::unique_ptr<DPFile> _bytecode;
	std::shared_ptr<DPFile> _bytecodeParameters;
};

}

#endif //AWE_COLLECTION_H
