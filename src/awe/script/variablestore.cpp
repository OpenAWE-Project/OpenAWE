//
// Created by patrick on 09.08.21.
//

#include "variablestore.h"

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
		throw std::runtime_error("Invalid variable id");
	_variables[id] = variable;
}

Variable VariableStore::getVariable(byte id) const {
	if (id > _variables.size())
		throw std::runtime_error("Invalid variable id");
	return _variables[id];
}

} // End of namespace AWE::Script
