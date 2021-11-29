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

#ifndef OPENAWE_DISASSEMBLER_H
#define OPENAWE_DISASSEMBLER_H

#include <map>

#include "src/common/readstream.h"

#include "src/awe/dpfile.h"

namespace AWE::Script {

class Disassembler {
public:
	Disassembler(
			Common::ReadStream *bytecode,
			std::shared_ptr<DPFile> parameters,
			const std::map<size_t, std::string>& entryPoints
	);

	std::string generate();

private:
	void push();
	void pushGID();
	void callGlobal(byte numArgs, byte retType);
	void callObject(byte numArgs, byte retType);
	void mulInt();
	void ret();
	void intToFloat();
	void setMember(byte id);
	void getMember(byte id);
	void cmp();
	void jmp();
	void jmpIf();
	void logAnd();
	void logOr();
	void logNot();
	void eq();
	void neq();

	std::string _disasm;
	std::unique_ptr<Common::ReadStream> _bytecode;
	std::shared_ptr<DPFile> _parameters;
	std::map<size_t, std::string> _entryPoints;
};

} // End of namespace AWE::Script

#endif //OPENAWE_DISASSEMBLER_H
