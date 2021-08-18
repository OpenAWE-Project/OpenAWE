//
// Created by patrick on 09.08.21.
//

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
