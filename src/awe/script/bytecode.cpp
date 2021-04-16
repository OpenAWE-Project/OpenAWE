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

#include <stdexcept>

#include <spdlog/spdlog.h>
#include <fmt/format.h>
#include <src/awe/types.h>

#include "bytecode.h"

enum Opcode {
	kPush       = 0x01,
	kPushGID    = 0x02,
	kCallGlobal = 0x03,
	kCallObject = 0x04,
	kRet        = 0x0D,
	kIntToFloat = 0x0E,
	kSetMember  = 0x0F,
	kGetMember  = 0x10,
	kCmp        = 0x13,
	kJmp        = 0x15,
	kJmpIf      = 0x1A,
	kLogAnd     = 0x1C,
	kLogOr      = 0x1D,
	kLogNot     = 0x1E,
	kNeq        = 0x24,
	kEq         = 0x25,
};

namespace AWE::Script {

Bytecode::Bytecode(Common::ReadStream *bytecode, const EntryPoints &entryPoints, std::shared_ptr<DPFile> parameters) :
	_bytecode(bytecode),
	_entryPoints(entryPoints),
	_parameters(parameters) {
}

bool Bytecode::hasEntryPoint(const std::string &entryPoint) {
	return _entryPoints.find(entryPoint) != _entryPoints.end();
}

void Bytecode::run(Context &context, const std::string &entryPoint, const entt::entity &caller) {
	spdlog::debug("Starting script entry point {}", entryPoint);
	auto entryPointIter = _entryPoints.find(entryPoint);
	if (entryPointIter == _entryPoints.end()) {
		//spdlog::warn("Entry point {} not found", entryPoint);
		return;
	}

	_bytecode->seek((*entryPointIter).second * 4);

	_eq = false;
	_gt = false;
	_lt = false;

	_stop = false;
	while (!_stop) {
		byte param1, param2, param3;

		param1 = _bytecode->readByte();
		param2 = _bytecode->readByte();
		param3 = _bytecode->readByte();

		auto opcode = Opcode(_bytecode->readByte());

		switch (opcode) {
			case kPush:       push(); break;
			case kPushGID:    pushGID(context); break;
			case kCallGlobal: callGlobal(context, caller, param1, param2); break;
			case kCallObject: callObject(context, param1, param2); break;
			case kRet:        ret(); break;
			case kIntToFloat: intToFloat(); break;
			case kCmp:        cmp(); break;
			case kJmp:        jmp(); break;
			case kJmpIf:      jmpIf(); break;
			case kLogAnd:     logAnd(); break;
			case kLogOr:      logOr(); break;
			case kLogNot:     logNot(); break;
			case kEq:         eq(); break;
			case kNeq:        neq(); break;
			default:
				throw std::runtime_error(fmt::format("Unknown opcode {:x}", opcode));
		}
	}

	spdlog::trace("Finishing script");
}

void Bytecode::push() {
	uint32_t data = _bytecode->readUint32LE();

	_stack.push(data);
	if (_parameters->hasString(data))
		spdlog::trace("push \"{}\"", _parameters->getString(data));
	else
		spdlog::trace("push {}", data);
}

void Bytecode::pushGID(Context &ctx) {
	GID gid;
	gid.type = _bytecode->readUint32LE();
	gid.id = _bytecode->readUint32BE();

	spdlog::trace("push_gid {} {:x}", gid.type, gid.id);

	_stack.push(ctx.getEntityByGID(gid));
}

void Bytecode::callGlobal(Context &ctx, const entt::entity &caller, byte numArgs, byte retType) {
	std::string object = _parameters->getString(std::get<uint32_t>(_stack.top()));
	_stack.pop();
	std::string method = _parameters->getString(std::get<uint32_t>(_stack.top()));
	_stack.pop();

	std::vector<Variable> arguments(numArgs);
	for (auto &argument : arguments) {
		if (_stack.empty())
			break;

		argument = _stack.top();
		_stack.pop();
	}

	std::optional<Variable> ret;
	// Call caller object when "this" is used
	if (object == "this")
		ret = ctx.getFunctions().callObject(caller, method, arguments);
	// If not call global object
	else
		ret = ctx.getFunctions().callGlobal(object, method, arguments);

	// Return variable if there is any
	if (ret)
		_stack.push(*ret);

	spdlog::trace("call_global {} {}", numArgs, retType);
}

void Bytecode::callObject(Context &ctx, byte numArgs, byte retType) {
	entt::entity entity = std::get<entt::entity>(_stack.top());
	_stack.pop();
	std::string method = _parameters->getString(std::get<uint32_t>(_stack.top()));
	_stack.pop();

	std::vector<Variable> arguments(numArgs);
	for (auto &argument : arguments) {
		argument = _stack.top();
		_stack.pop();
	}

	auto ret = ctx.getFunctions().callObject(entity, method, arguments);
	if (ret)
		_stack.push(*ret);

	spdlog::trace("call_object {} {}", numArgs, retType);
}

void Bytecode::ret() {
	_stop = true;
	
	spdlog::trace("ret");
}

void Bytecode::intToFloat() {
	uint32_t value = std::get<uint32_t>(_stack.top());
	_stack.pop();

	auto fValue = static_cast<float>(value);
	std::memcpy(&value, &fValue, 4);
	_stack.push(value);

	spdlog::trace("int_to_float");
}

void Bytecode::cmp() {
	uint32_t value1 = std::get<uint32_t>(_stack.top());
	_stack.pop();
	uint32_t value2 = std::get<uint32_t>(_stack.top());
	_stack.pop();

	_eq = value1 == value2;
	_gt = value1 > value2;
	_lt = value1 < value2;

	spdlog::trace("cmp");
}

void Bytecode::jmp() {
	uint32_t offset = _bytecode->readUint32LE();
	_bytecode->skip(offset * 4);

	spdlog::trace("jmp {}", offset);
}

void Bytecode::jmpIf() {
	uint32_t offset = _bytecode->readUint32LE();
	if (_eq)
		_bytecode->skip(offset * 4);

	spdlog::trace("jmp_if {}", offset);
}

void Bytecode::logAnd() {
	bool value1 = std::get<uint32_t>(_stack.top()) != 0;
	_stack.pop();
	bool value2 = std::get<uint32_t>(_stack.top()) != 0;
	_stack.pop();

	_stack.push((value1 && value2) ? 1u : 0u);

	spdlog::trace("and");
}

void Bytecode::logOr() {
	bool value1 = std::get<uint32_t>(_stack.top()) != 0;
	_stack.pop();
	bool value2 = std::get<uint32_t>(_stack.top()) != 0;
	_stack.pop();

	_stack.push((value1 || value2) ? 1u : 0u);

	spdlog::trace("or");
}

void Bytecode::logNot() {
	bool value = std::get<uint32_t>(_stack.top()) != 0;
	_stack.pop();

	_stack.push(!value ? 1u : 0u);

	spdlog::trace("not");
}

void Bytecode::neq() {
	_stack.push(!_eq);

	spdlog::trace("neq");
}

void Bytecode::eq() {
	_stack.push(_eq);

	spdlog::trace("eq");
}

}
