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

#include <spdlog/spdlog.h>

#include "src/awe/script/collection.h"

namespace AWE::Script {

Collection::Collection(Common::ReadStream *bytecode, Common::ReadStream *bytecodeParameters) :
		_bytecode(new DPFile(bytecode)),
		_bytecodeParameters(new DPFile(bytecodeParameters)) {
}

Bytecode *Collection::createScript(const AWE::CIDFile::Script &script) {
	std::vector<DPFile::ScriptMetadata> metadata = _bytecode->getScriptMetadata(script.offsetHandlers, script.numHandlers);
	std::vector<DPFile::ScriptSignal> signals = _bytecode->getScriptSignals(script.offsetSignals, script.numSignals);

	EntryPoints entryPoints;
	for (const auto &item : metadata) {
		std::string handler = _bytecodeParameters->getString(item.name);
		spdlog::debug("Add script entry point {}", handler);

		assert(item.offset <= script.codeSize);

		entryPoints[handler] = item.offset;
	}

	for (const auto &signal : signals) {
		spdlog::debug("Add script signal {}", _bytecodeParameters->getString(signal.nameOffset));
	}

	assert(_bytecodeParameters);

	return new Bytecode(
			_bytecode->getStream(script.offsetCode, script.codeSize),
			entryPoints,
			_bytecodeParameters
	);
}

}
