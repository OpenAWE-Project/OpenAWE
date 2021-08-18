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

#ifndef AWE_BYTECODE_H
#define AWE_BYTECODE_H

#include <map>
#include <string>
#include <memory>
#include <stack>
#include <variant>

#include "src/common/readstream.h"

#include "src/awe/dpfile.h"
#include "src/awe/script/types.h"
#include "src/awe/script/context.h"

namespace AWE::Script {

class Bytecode;

typedef std::shared_ptr<Bytecode> BytecodePtr;
typedef std::map<std::string, uint32_t> EntryPoints;
typedef std::map<uint32_t, std::string> DebugEntries;

class Bytecode : Common::Noncopyable {
public:
	Bytecode(Common::ReadStream *bytecode, const EntryPoints &entryPoints, std::shared_ptr<DPFile> parameters);

	/*!
	 * Check if a specific entry point is available
	 * \param entryPoint the entry point to check for
	 * \return if this entry point exists in this byte code
	 */
	bool hasEntryPoint(const std::string &entryPoint);
	void run(Context &context, const std::string &entryPoint, const entt::entity &caller);

private:
	void push();
	void pushGID(Context &ctx);
	void callGlobal(Context &ctx, const entt::entity &caller, byte numArgs, byte retType);
	void callObject(Context &ctx, byte numArgs, byte retType);
	void ret();
	void intToFloat();
	void setMember(Context &ctx, byte id);
	void getMember(Context &ctx, byte id);
	void cmp();
	void jmp();
	void jmpIf();
	void logAnd();
	void logOr();
	void logNot();
	void neq();
	void eq();

	bool _gt, _lt, _eq;

	bool _stop;
	std::unique_ptr<Common::ReadStream> _bytecode;
	std::shared_ptr<DPFile> _parameters;
	std::stack<Variable> _stack;
	const EntryPoints _entryPoints;
};

} // End of namespace

#endif //AWE_BYTECODE_H
