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

#include <fmt/format.h>
#include <src/awe/types.h>

#include <utility>

#include "src/awe/script/disassembler.h"
#include "src/awe/script/types.h"

namespace AWE::Script {

Disassembler::Disassembler(
		Common::ReadStream *bytecode,
		std::shared_ptr<DPFile> dpFile,
		const std::map<size_t, std::string> &entryPoints
) : _bytecode(bytecode), _parameters(dpFile), _entryPoints(entryPoints) {
}

std::string Disassembler::generate() {
	_disasm.clear();
	while (!_bytecode->eos()) {
		if (_entryPoints.find(_bytecode->pos()) != _entryPoints.end()) {
			_disasm += fmt::format("# {}()\n", _entryPoints[_bytecode->pos()]);
		}

		byte param1, param2, param3;

		param1 = _bytecode->readByte();
		param2 = _bytecode->readByte();
		param3 = _bytecode->readByte();

		auto opcode = Opcode(_bytecode->readByte());

		switch (opcode) {
			case kPush:       push(); break;
			case kPushGID:    pushGID(); break;
			case kCallGlobal: callGlobal(param1, param2); break;
			case kCallObject: callObject(param1, param2); break;
			case kMulInt:     mulInt(); break;
			case kRet:        ret(); break;
			case kIntToFloat: intToFloat(); break;
			case kSetMember:  setMember(param1); break;
			case kGetMember:  getMember(param1); break;
			case kCmp:        cmp(); break;
			case kJmp:        jmp(); break;
			case kJmpIf:      jmpIf(); break;
			case kLogAnd:     logAnd(); break;
			case kLogOr:      logOr(); break;
			case kLogNot:     logNot(); break;
			case kEq:         eq(); break;
			case kNeq:        neq(); break;
			default:
				_disasm += fmt::format("unk {:x}", opcode);
		}
	}

	return _disasm;
}

void Disassembler::push() {
	int32_t value = _bytecode->readSint32LE();
	if (_parameters->hasString(value)) {
		_disasm += fmt::format("push \"{}\"\n", _parameters->getString(value));
	} else {
		_disasm += fmt::format("push {}\n", value);
	}
}

void Disassembler::pushGID() {
	GID gid;
	gid.type = _bytecode->readUint32LE();
	gid.id = _bytecode->readUint32BE();

	_disasm += fmt::format("push_gid {} {:x}\n", gid.type, gid.id);
}

void Disassembler::callGlobal(byte numArgs, byte retType) {
	_disasm += fmt::format("call_global {} {}\n", numArgs, retType);
}

void Disassembler::callObject(byte numArgs, byte retType) {
	_disasm += fmt::format("call_object {} {}\n", numArgs, retType);
}

void Disassembler::mulInt() {
	_disasm += "mul_int\n";
}

void Disassembler::ret() {
	_disasm += "ret\n";
}

void Disassembler::intToFloat() {
	_disasm += "intToFloat\n";
}

void Disassembler::setMember(byte id) {
	_disasm += fmt::format("set_member {}\n", id);
}

void Disassembler::getMember(byte id) {
	_disasm += fmt::format("get_member {}\n", id);
}

void Disassembler::cmp() {
	_disasm += "cmp\n";
}

void Disassembler::jmp() {
	int32_t offset = _bytecode->readSint32LE();
	_disasm += fmt::format("jmp {}\n", offset);
}

void Disassembler::jmpIf() {
	int32_t offset = _bytecode->readSint32LE();
	_disasm += fmt::format("jmp_if {}\n", offset);
}

void Disassembler::logAnd() {
	_disasm += "and\n";
}

void Disassembler::logOr() {
	_disasm += "or\n";
}

void Disassembler::logNot() {
	_disasm += "not\n";
}

void Disassembler::eq() {
	_disasm += "eq\n";
}

void Disassembler::neq() {
	_disasm += "neq\n";
}

} // End of namespace AWE::Script
