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

#ifndef OPENAWE_DXBC_H
#define OPENAWE_DXBC_H

#include <array>
#include <variant>
#include <set>
#include <bitset>
#include <optional>

#include <fmt/args.h>

#include "src/common/readstream.h"

namespace Common {

enum class SourceModifier {
	kNone,
	kNegate,
	kBias,
	kBiasAndNegate,
	kSign,
	kSignAndNegate,
	kComplement,
	kX2,
	kX2AndNegate,
	kDivideZ,
	kDivideW,
	kAbs,
	kAbsAndNegate
};

enum RegisterType {
	kTemp,
	kInput,
	kConst,
	kAddr,
	kTexture,
	kRestOut,
	kAttrOut,
	kTexCRDOut,
	kOutput,
	kConstInt,
	kColorOut,
	kDepthOut,
	kSampler,
	kConst2,
	kConst3,
	kConst4,
	kConstBool,
	kLoop,
	kTempFloat16,
	kMiscType,
	kLabel,
	kPredicate,
	kForceDword
};

enum class Direction {
	kIn, kOut, kPass
};

enum class Type {
	kFloat,
	kVec2,
	kVec3,
	kVec4,
	kMat2,
	kMat3,
	kMat4,
	kMat4x3,
	kTexture2D,
	kTexture3D,
	kTextureCube,
	kStruct
};

enum class UniformType {
	kValue,
	kSampler
};

struct Destination {
	unsigned int id;
	std::string name;
	std::string swizzle;
	RegisterType type;
	bool saturate, partialPrecision, centroid;
	std::bitset<4> mask;
};

struct Source {
	std::string name;
	std::string swizzle;
	RegisterType type;
	SourceModifier modifier;
	std::optional<Type> samplerType;
};

struct Constant {
	std::string name;
	glm::vec4 value;
};

struct Uniform {
	std::string name;
	Type type;
	unsigned int length{1};
	std::vector<Uniform> members;
};

struct Attribute {
	std::string name;
	Type type;
};

template<size_t NumSources>
struct Instruction {
	Destination dst;
	std::array<Source, NumSources> src;
};

template<size_t NumSources>
struct SourceOnlyInstruction {
	std::array<Source, NumSources> src;
};

struct EmptyInstruction {};

struct Mov : Instruction<1> {};
struct Add : Instruction<2> {};
struct Sub : Instruction<2> {};
struct Mad : Instruction<3> {};
struct Mul : Instruction<2> {};
struct Rcp : Instruction<1> {};
struct Rsq : Instruction<1> {};
struct Dp3 : Instruction<2> {};
struct Dp4 : Instruction<2> {};
struct Min : Instruction<2> {};
struct Max : Instruction<2> {};
struct Slt : Instruction<2> {};
struct Exp : Instruction<1> {};
struct Log : Instruction<1> {};
struct Lrp : Instruction<3> {};
struct Frc : Instruction<1> {};
struct Pow : Instruction<2> {};
struct Abs : Instruction<1> {};
struct Nrm : Instruction<1> {};
struct SinCos : Instruction<1> {};
struct Ifc : SourceOnlyInstruction<2> {};
struct Else : EmptyInstruction {};
struct EndIf : EmptyInstruction {};
struct Tex : Instruction<2> {};
struct Cmp : Instruction<3> {};
struct Dp2Add : Instruction<3>{};

typedef std::variant<Mov, Add, Sub, Mad, Mul, Rcp, Rsq, Dp3, Dp4, Min, Max, Slt, Exp, Log, Lrp, Frc, Pow, Abs, Nrm, SinCos, Ifc, Else, EndIf, Tex, Cmp, Dp2Add> Inst;

/**
 * \brief Decompiler for directx 9 bytecode
 *
 * This class can parse directx 9 bytecode and translate it to other formats. It can write them back as glsl source
 * code.
 */
class DXBC {
public:
	DXBC(Common::ReadStream &dxbc);

	/**
	 * Return the string for the generated glsl source code
	 * \return The string containing the glsl source code
	 */
	std::string generateGlsl();

private:

	std::string _creator, _target;
	Direction _in, _out;
	std::vector<Constant> _constants;
	std::set<unsigned int> _tempVariables;
	std::vector<Inst> _instructions;
	std::vector<Uniform> _uniforms;

	std::map<unsigned int, std::string> _uniformNames;
	std::map<unsigned int, std::string> _uniformTextureNames;
	std::map<unsigned int, std::string> _attributeInNames;
	std::map<unsigned int, std::string> _attributeOutNames;

	std::string formatRegisterName(RegisterType type, unsigned int number);

	Source readSource(Common::ReadStream &dxbc);
	Destination readDestination(Common::ReadStream &dxbc);

	void readAttribute(ReadStream &dxbc);
	void readConstants(ReadStream &dxbc);

	template<typename I>
	Inst readInstruction(Common::ReadStream &dxbc) {
		I instruction;
		instruction.dst = readDestination(dxbc);
		for (auto &src: instruction.src) {
			src = readSource(dxbc);
		}

		return instruction;
	}

	template<typename I>
	Inst readSourceOnlyInstruction(Common::ReadStream &dxbc) {
		I instruction;
		for (auto &src: instruction.src) {
			src = readSource(dxbc);
		}

		return instruction;
	}

};

} // End of namespace Common

#endif //OPENAWE_DXBC_H
