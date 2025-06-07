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

#include <map>

#include "src/common/exception.h"
#include "src/common/bitstream.h"

#include "dxbc.h"

static constexpr uint32_t kCTAB = MKTAG('C', 'T', 'A', 'B');

enum DXBCType {
	kVertex = 0xFFFE,
	kPixel = 0xFFFF
};

enum Opcode {
	kNOP = 0,
	kMOV = 1,
	kADD = 2,
	kSUB = 3,
	kMAD = 4,
	kMUL = 5,
	kRCP = 6,
	kRSQ = 7,
	kDP3 = 8,
	kDP4 = 9,
	kMIN = 10,
	kMAX = 11,
	kSLT = 12,
	kSGE = 13,
	kEXP = 14,
	kLOG = 15,
	kLIT = 16,
	kDST = 17,
	kLRP = 18,
	kFRC = 19,
	kM4x4 = 20,
	kM4x3 = 21,
	kM3x4 = 22,
	kM3x3 = 23,
	kM3x2 = 24,
	kCALL = 25,
	kCALLNZ = 26,
	kLOOP = 27,
	kRET = 28,
	kENDLOOP = 29,
	kLABEL = 30,
	kDCL = 31,
	kPOW = 32,
	kCRS = 33,
	kSGN = 34,
	kABS = 35,
	kNRM = 36,
	kSINCOS = 37,
	kREP = 38,
	kENDREP = 39,
	kIF = 40,
	kIFC = 41,
	kELSE = 42,
	kENDIF = 43,
	kBREAK = 44,
	kBREAKC = 45,
	kMOVA = 46,
	kDEFB = 47,
	kDEFI = 48,

	kTEXCOORD = 64,
	kTEXKILL = 65,
	kTEX = 66,
	kTEXBEM = 67,
	kTEXBEML = 68,
	kTEXREG2AR = 69,
	kTEXREG2GB = 70,
	kTEXM3x2PAD = 71,
	kTEXM3x2TEX = 72,
	kTEXM3x3PAD = 73,
	kTEXM3x3TEX = 74,
	kTEXM3x3DIFF = 75,
	kTEXM3x3SPEC = 76,
	kTEXM3x3VSPEC = 77,
	kEXPP = 78,
	kLOGP = 79,
	kCND = 80,
	kDEF = 81,
	kTEXREG2RGB = 82,
	kTEXDP3TEX = 83,
	kTEXM3x2DEPTH = 84,
	kTEXDP3 = 85,
	kTEXM3x3 = 86,
	kTEXDEPTH = 87,
	kCMP = 88,
	kBEM = 89,
	kDP2ADD = 90,
	kDSX = 91,
	kDSY = 92,
	kTEXLDD = 93,
	kSETP = 94,
	kTEXLDL = 95,
	kBREAKP = 96,

	kComment = 0xFFFE,
	kEnd = 0xFFFF
};

enum SwizzleValue {
	X, Y, Z, W
};

enum Usage {
	kPosition,
	kBlendWeight,
	kBlendIndices,
	kNormal,
	kPSize,
	kTexCoord,
	kTangent,
	kBinormal,
	kTessFactor,
	kPositionT,
	kColor,
	kFog,
	kDepth,
	kSample
};

enum Class {
	kScalar,
	kVector,
	kMatrixRows,
	kMatrixColumns,
	kObject,
	kStruct
};

enum VariableType {
	kTypeVoid,
	kTypeBool,
	kTypeInt,
	kTypeFloat,
	kTypeString,
	kTypeTexture,
	kTypeTexture1D,
	kTypeTexture2D,
	kTypeTexture3D,
	kTypeTextureCube,
	kTypeSampler,
	kTypeSampler1D,
	kTypeSampler2D,
	kTypeSampler3D,
	kTypeSamplerCube,
};

enum Modifier {
	kNone,
	kAbs,
	kNormalize,
	kExp,
	kReciprocal,
	kReciprocalSqrt,
};

struct Register {
	std::string name;
	unsigned int size;
	std::vector<SwizzleValue> swizzle;
};

struct TypeInfo {
	unsigned int numRows, numColumns, numElements;
	Class typeClass;
	VariableType type;
};

struct ConstantStructMember {
	std::string name;
	TypeInfo info;
};

struct ConstantInfo {
	std::string name;
	unsigned int index, registerCount;
	TypeInfo info;
	std::vector<ConstantStructMember> members;
};

constexpr uint32_t gen_lh_bitmask(uint8_t size) {
	uint32_t bitmask = 0;
	for (int i = 0; i < size; ++i) {
		bitmask = (bitmask << 1) | 1;
	}

	return bitmask;
}

namespace Common {

std::string formatDestination(const Destination &dst, const std::bitset<4> &mask) {
	if (dst.swizzle == "xyzw")
		return dst.name;

	return std::format("{}.{}", dst.name, dst.swizzle);
}

std::string formatSource(const Source &src, const std::bitset<4> &mask) {
	std::string srcToken = src.name;
	if (!src.swizzle.empty()) {
		std::string srcSwizzle = "";
		for (size_t i = 0; i < mask.size(); ++i) {
			if (mask.test(i))
				srcSwizzle += src.swizzle.at(i);
		}

		srcToken = std::format("{}.{}", src.name, srcSwizzle);
	}

	switch (src.modifier) {
		case SourceModifier::kNone:
			break; // Do Nothing

		case SourceModifier::kNegate:
			srcToken = std::format("-{}", srcToken);
			break;

		case SourceModifier::kAbs:
			srcToken = std::format("abs({})", srcToken);
			break;

		case SourceModifier::kAbsAndNegate:
			srcToken = std::format("-abs({})", srcToken);
			break;

		case SourceModifier::kBiasAndNegate:
			break; // TODO

		default:
			throw CreateException("Unsupported Modifier");
	}

	return srcToken;
}

template<size_t NumSources>
std::string formatInstruction(const std::string &str, SourceOnlyInstruction<NumSources> inst) {
	fmt::dynamic_format_arg_store<fmt::format_context> args{};
	for (const auto &src: inst.src) {
		args.push_back(formatSource(src, 0b1111));
	}
	return fmt::vformat(str, args);
}

template<size_t NumSources>
std::string formatInstruction(const std::string &str, Instruction<NumSources> inst) {
	fmt::dynamic_format_arg_store<fmt::format_context> args{};
	for (const auto &src: inst.src) {
		args.push_back(formatSource(src, inst.dst.mask));
	}

	auto rhand = fmt::vformat(str, args);
	if (inst.dst.saturate)
		rhand = std::format("clamp({}, 0.0, 1.0)", rhand);

	//assert(!inst.dst.centroid /*&& !inst.dst.partialPrecision*/);

	return std::format("{} = {};\n", formatDestination(inst.dst, inst.dst.mask), rhand);
}

template<size_t NumSources>
std::string formatInstruction(const std::string &str, Instruction<NumSources> inst, uint8_t dstSwizzleLength,
							  uint8_t srcSwizzleLength) {
	fmt::dynamic_format_arg_store<fmt::format_context> args{};

	const auto dstMask = gen_lh_bitmask(dstSwizzleLength);
	const auto srcMask = gen_lh_bitmask(srcSwizzleLength);

	for (const auto &src: inst.src) {
		args.push_back(formatSource(src, srcMask));
	}

	auto rhand = fmt::vformat(str, args);
	if (inst.dst.saturate)
		rhand = std::format("clamp({}, 0.0, 1.0)", rhand);

	assert(!inst.dst.centroid && !inst.dst.partialPrecision);

	return std::format("{} = {};\n", formatDestination(inst.dst, dstMask), rhand);
}

template<size_t NumSources>
std::string formatInstruction(const std::string &str, Instruction<NumSources> inst, uint8_t dstSwizzleLength,
							  std::vector<uint8_t> srcSwizzleLengths) {
	fmt::dynamic_format_arg_store<fmt::format_context> args{};

	const auto dstMask = gen_lh_bitmask(std::min<uint8_t>(dstSwizzleLength, inst.dst.swizzle.size()));

	for (size_t i = 0; i < inst.src.size(); ++i) {
		const auto srcMask = gen_lh_bitmask(srcSwizzleLengths.at(i));
		args.push_back(formatSource(inst.src[i], srcMask));
	}

	auto rhand = fmt::vformat(str, args);
	if (inst.dst.saturate)
		rhand = std::format("clamp({}, 0.0, 1.0)", rhand);

	assert(!inst.dst.centroid && !inst.dst.partialPrecision);

	return std::format("{} = {};\n", formatDestination(inst.dst, dstMask), rhand);
}

std::string formatDeclarationName(Usage usage, Direction direction, unsigned int index) {
	std::string prefix;
	switch (direction) {
		using
		enum Direction;
		case kIn:
			prefix = "in";
			break;
		case kOut:
			prefix = "out";
			break;
		case kPass:
			prefix = "pass";
			break;
	}

	switch (usage) {
		case kPosition:
			return std::format("{}_Position", prefix);
		case kNormal:
			return std::format("{}_Normal", prefix);
		case kTexCoord:
			return std::format("{}_UV{}", prefix, index);
		case kColor:
			return std::format("{}_Color{}", prefix, index);
		default:
			return std::format("{}_Unknown", prefix);
	}
}

std::string getTypeString(Type type) {
	switch (type) {
		using
		enum Type;
		case kFloat:
			return "float";
		case kVec2:
			return "vec2";
		case kVec3:
			return "vec3";
		case kVec4:
			return "vec4";
		case kMat3:
			return "mat3";
		case kMat4:
			return "mat4";
		case kTexture2D:
			return "sampler2D";
		case kTexture3D:
			return "sampler3D";
		case kTextureCube:
			return "samplerCube";
		case kStruct:
			return "struct";
		default:
			return "unknown";
	}
}

DXBC::DXBC(Common::ReadStream &dxbc) {
	// Parse Version
	const uint8_t minor = dxbc.readByte();
	const uint8_t major = dxbc.readByte();

	if (major != 3 || minor != 0)
		throw CreateException("Invalid dxbc version {}.{}, we only support 3.0", major, minor);

	const auto type = dxbc.readUint16LE();
	if (type != 0xFFFE && type != 0xFFFF)
		throw CreateException("Unknown dxbc type {:X}, neither pixel nor vertex shader", type);

	using
	enum Direction;
	if (type == 0xFFFE) { // Vertex Shader
		_in = kIn;
		_out = kPass;
	} else if (type == 0xFFFF) { // Pixel Shader
		_in = kPass;
		_out = kOut;
	}

	Common::BitStream8LSB bits(dxbc);

	while (!dxbc.eos()) {

		const auto token = Opcode(dxbc.readUint16LE());

		switch (token) {
			case kComment: {
				const auto length = dxbc.readUint16LE() * 4;

				std::unique_ptr<Common::ReadStream> comment(dxbc.readStream(length));
				const auto magic = comment->readUint32BE();

				if (magic != kCTAB)
					throw CreateException("Expected CTAB");

				const auto size = comment->readUint32LE();
				const auto creatorOffset = comment->readUint32LE();
				const auto version = comment->readUint32BE();
				const auto numConstants = comment->readUint32LE();
				const auto constantsInfoOffset = comment->readUint32LE();
				const auto flags = comment->readUint32LE();
				const auto targetOffset = comment->readUint32LE();

				assert(size == 28);

				comment->seek(creatorOffset + 4);
				_creator = comment->readNullTerminatedString();

				comment->seek(targetOffset + 4);
				_target = comment->readNullTerminatedString();

				comment->seek(constantsInfoOffset + 4);
				for (unsigned int i = 0; i < numConstants; ++i) {
					ConstantInfo info{};

					const auto nameOffset = comment->readUint32LE();
					const auto registerSet = comment->readUint16LE();
					info.index = comment->readUint16LE();
					info.registerCount = comment->readUint16LE();

					comment->skip(2);

					const auto typeInfoOffset = comment->readUint32LE();
					const auto defaultValue = comment->readUint32LE();

					const auto lastPos = comment->pos();
					comment->seek(typeInfoOffset + 4);

					info.info.typeClass = Class(comment->readUint16LE());
					info.info.type = VariableType(comment->readUint16LE());
					info.info.numRows = comment->readUint16LE();
					info.info.numColumns = comment->readUint16LE();
					info.info.numElements = comment->readUint16LE();

					// Struct members
					const auto numStructMembers = comment->readUint16LE();
					const auto structMembersOffset = comment->readUint32LE();

					comment->seek(structMembersOffset + 4);
					info.members.resize(numStructMembers);
					for (auto &member: info.members) {
						const auto memberNameOffset = comment->readUint32LE();
						const auto typeInfoOffset = comment->readUint32LE();

						const auto lastMemberPosition = comment->pos();

						comment->seek(memberNameOffset + 4);
						member.name = comment->readNullTerminatedString();

						comment->seek(typeInfoOffset + 4);
						member.info.typeClass = Class(comment->readUint16LE());
						member.info.type = VariableType(comment->readUint16LE());
						member.info.numRows = comment->readUint16LE();
						member.info.numColumns = comment->readUint16LE();
						member.info.numElements = comment->readUint16LE();

						// We assume there are no structs in structs

						comment->seek(lastMemberPosition);
					}

					comment->seek(nameOffset + 4);
					info.name = comment->readNullTerminatedString();

					comment->seek(lastPos);

					for (unsigned int j = 0; j < info.registerCount; ++j) {
						switch (info.info.type) {
							case kTypeSampler1D:
								[[fallthrough]];
							case kTypeSampler2D:
								[[fallthrough]];
							case kTypeSampler3D:
								[[fallthrough]];
							case kTypeSamplerCube:
								if (info.registerCount > 1)
									_uniformTextureNames[info.index + j] = std::format("{}[{}]", info.name, j);
								else
									_uniformTextureNames[info.index + j] = info.name;
								break;

							case kTypeVoid:
								_uniformNames[info.index + j] = std::format("{}.{}", info.name, info.members[j].name);
								break;

							default:
								if (info.registerCount > 1)
									_uniformNames[info.index + j] = std::format("{}[{}]", info.name, j);
								else
									_uniformNames[info.index + j] = info.name;
								break;
						}
					}

					Type utype;
					switch (info.info.typeClass) {
						using
						enum Type;
						case kScalar:
							utype = kFloat;
							break;
						case kVector:
							switch (info.info.numColumns) {
								case 2:
									utype = kVec2;
									break;
								case 3:
									utype = kVec3;
									break;
								case 4:
									utype = kVec4;
									break;
								default:
									throw CreateException("Invalid vector size {}, only 2, 3 and 4 allowed",
														  info.info.numRows);
							}
							break;
						case kMatrixRows:
							if (info.info.numRows == info.info.numColumns) {
								switch (info.info.numRows) {
									case 2:
										utype = kMat2;
										break;
									case 3:
										utype = kMat3;
										break;
									case 4:
										utype = kMat4;
										break;
									default:
										throw CreateException("Invalid matrix size {}, only 2, 3 and 4 allowed",
															  info.info.numRows);
								}
							}
							break;
						case Class::kStruct:
							utype = kStruct;
							break;
						case Class::kObject:
							switch (info.info.type) {
								case kTypeSampler2D:
									utype = kTexture2D;
									break;
								case kTypeSampler3D:
									utype = kTexture3D;
									break;
								case kTypeSamplerCube:
									utype = kTextureCube;
									break;
								default:
									throw CreateException("Invalid sampler type {}", fmt::underlying(info.info.type));
							}
							break;

						default:
							throw CreateException("Invalid uniform type");
					}

					auto &uniform = _uniforms.emplace_back(info.name, utype, info.registerCount);

					for (const auto &member: info.members) {
						Type utype;
						switch (member.info.typeClass) {
							using
							enum Type;
							case kScalar:
								utype = kFloat;
								break;
							case kVector:
								switch (member.info.numColumns) {
									case 2:
										utype = kVec2;
										break;
									case 3:
										utype = kVec3;
										break;
									case 4:
										utype = kVec4;
										break;
									default:
										throw CreateException("Invalid vector size {}, only 2, 3 and 4 allowed",
															  info.info.numRows);
								}
								break;
							case kMatrixRows:
								if (info.info.numRows == info.info.numColumns) {
									switch (member.info.numRows) {
										case 2:
											utype = kMat2;
											break;
										case 3:
											utype = kMat3;
											break;
										case 4:
											utype = kMat4;
											break;
										default:
											throw CreateException("Invalid matrix size {}, only 2, 3 and 4 allowed",
																  info.info.numRows);
									}
								}
								break;
							case Class::kStruct:
								utype = kStruct;
								break;
							case Class::kObject:
								switch (member.info.type) {
									case kTypeSampler2D:
										utype = kTexture2D;
										break;
									case kTypeSampler3D:
										utype = kTexture3D;
										break;
									case kTypeSamplerCube:
										utype = kTextureCube;
										break;
									default:
										throw CreateException("Unexpected type {}", fmt::underlying(member.info.type));
								}
								break;

							default:
								throw CreateException("Invalid uniform type");
						}

						uniform.members.emplace_back(member.name, utype);
					}
				}

				continue;
			}

			case kEnd: {
				const auto zero = dxbc.readUint16LE();
				assert(zero == 0);
				continue;
			}

			default:
				break; // Every other token will be handled as instruction
		}

		const auto controls = dxbc.readByte();
		const auto instructionSize = bits.read(4);
		const auto predicated = bits.read();
		const auto zero = bits.read(3);

		switch (token) {
			case kMOV:
				_instructions.emplace_back(readInstruction<Mov>(dxbc));
				break;
			case kADD:
				_instructions.emplace_back(readInstruction<Add>(dxbc));
				break;
			case kSUB:
				_instructions.emplace_back(readInstruction<Sub>(dxbc));
				break;
			case kMAD:
				_instructions.emplace_back(readInstruction<Mad>(dxbc));
				break;
			case kMUL:
				_instructions.emplace_back(readInstruction<Mul>(dxbc));
				break;
			case kRCP:
				_instructions.emplace_back(readInstruction<Rcp>(dxbc));
				break;
			case kRSQ:
				_instructions.emplace_back(readInstruction<Rsq>(dxbc));
				break;
			case kDP3:
				_instructions.emplace_back(readInstruction<Dp3>(dxbc));
				break;
			case kDP4:
				_instructions.emplace_back(readInstruction<Dp4>(dxbc));
				break;
			case kMIN:
				_instructions.emplace_back(readInstruction<Min>(dxbc));
				break;
			case kMAX:
				_instructions.emplace_back(readInstruction<Max>(dxbc));
				break;
			case kSLT:
				_instructions.emplace_back(readInstruction<Slt>(dxbc));
				break;
			case kEXP:
				_instructions.emplace_back(readInstruction<Exp>(dxbc));
				break;
			case kLOG:
				_instructions.emplace_back(readInstruction<Log>(dxbc));
				break;
			case kLRP:
				_instructions.emplace_back(readInstruction<Lrp>(dxbc));
				break;
			case kFRC:
				_instructions.emplace_back(readInstruction<Frc>(dxbc));
				break;
			case kDCL:
				readAttribute(dxbc);
				break;
			case kPOW:
				_instructions.emplace_back(readInstruction<Pow>(dxbc));
				break;
			case kABS:
				_instructions.emplace_back(readInstruction<Abs>(dxbc));
				break;
			case kNRM:
				_instructions.emplace_back(readInstruction<Nrm>(dxbc));
				break;
			case kSINCOS:
				_instructions.emplace_back(readInstruction<SinCos>(dxbc));
				break;
			case kIFC:
				_instructions.emplace_back(readSourceOnlyInstruction<Ifc>(dxbc));
				break;
			case kELSE:
				_instructions.emplace_back(Else());
				break;
			case kENDIF:
				_instructions.emplace_back(EndIf());
				break;
			case kTEX:
				_instructions.emplace_back(readInstruction<Tex>(dxbc));
				break;
			case kDEF:
				readConstants(dxbc);
				break;
			case kCMP:
				_instructions.emplace_back(readInstruction<Cmp>(dxbc));
				break;
			case kDP2ADD:
				_instructions.emplace_back(readInstruction<Dp2Add>(dxbc));
				break;

			default:
				throw CreateException("Unimplemented dxbc opcode {}", fmt::underlying(token));
		}
	}

	fmt::print("{}", generateGlsl());
}

std::string DXBC::generateGlsl() {
	std::string source;

	source += std::format("// {}\n", _creator);
	source += std::format("// {}\n", _target);

	source += "#version 330\n\n";

	// Print uniforms
	for (const auto &uniform: _uniforms) {
		if (uniform.type == Type::kStruct) {
			source += "uniform struct {\n";

			for (const auto &member: uniform.members) {
				source += std::format("\t{} {};\n", getTypeString(member.type), member.name);
			}

			source += std::format("}} {};\n", uniform.name);
		} else {
			auto length = uniform.length;
			switch (uniform.type) {
				case Type::kMat2:
					length /= 2;
					break;
				case Type::kMat4x3:
					[[fallthrough]];
				case Type::kMat3:
					length /= 3;
					break;
				case Type::kMat4:
					length /= 4;
					break;
				default:
					break;
			}

			length = std::max(length, 1u);

			if (length == 1)
				source += std::format("uniform {} {};\n", getTypeString(uniform.type), uniform.name);
			else
				source += std::format("uniform {} {}[{}];\n", getTypeString(uniform.type), uniform.name,
									  uniform.length);
		}
	}

	source += "\n";

	// Print attributes
	if (!_attributeInNames.empty()) {
		for (const auto &[_, attr]: _attributeInNames) {
			if (attr == "pass_Position") {
				source += "#define pass_Position gl_Position\n";
				continue;
			}

			source += std::format("in vec4 {};\n", attr);
		}

		source += "\n";
	}

	if (!_attributeOutNames.empty()) {
		for (const auto &[_, attr]: _attributeOutNames) {
			if (attr == "pass_Position") {
				source += "#define pass_Position gl_Position\n";
				continue;
			}

			source += std::format("out vec4 {};\n", attr);
		}

		source += "\n";
	}

	// Print constants
	for (const auto &constant: _constants) {
		source += std::format(
				"const vec4 {} = vec4({}, {}, {}, {});\n",
				constant.name,
				constant.value.x, constant.value.y, constant.value.z, constant.value.w
		);
	}

	source += "\n";

	// Print actual code
	source += "void main() {\n";

	// Print temp variables
	for (const auto &variable: _tempVariables) {
		source += std::format("\tvec4 t{};\n", variable);
	}

	source += "\n";

	size_t indent = 1;
	for (const auto &instruction: _instructions) {
		for (size_t i = 0; i < indent; ++i) {
			source += "\t";
		}

		source += std::visit(Common::Overloaded{
				[](const Mov &mov) { return formatInstruction("{}", mov); },
				[](const Add &add) { return formatInstruction("{} + {}", add); },
				[](const Sub &sub) { return formatInstruction("{} - {}", sub); },
				[](const Mad &mad) { return formatInstruction("({} * {}) + {}", mad); },
				[](const Mul &mul) { return formatInstruction("{} * {}", mul); },
				[](const Rcp &rcp) { return formatInstruction("1.0 / {}", rcp); },
				[](const Rsq &rsq) { return formatInstruction("inversesqrt(abs({}))", rsq); },
				[](const Dp3 &dp3) { return formatInstruction("dot({}, {})", dp3, 1, 3); },
				[](const Dp4 &dp4) { return formatInstruction("dot({}, {})", dp4, 1, 4); },
				[](const Min &min) { return formatInstruction("min({}, {})", min); },
				[](const Max &max) { return formatInstruction("max({}, {})", max); },
				[](const Slt &slt) { return formatInstruction("float({} < {})", slt); },
				[](const Exp &exp) { return formatInstruction("exp2({})", exp); },
				[](const Log &log) { return formatInstruction("log2({})", log); },
				[](const Lrp &lrp) {
					return formatInstruction("mix({2}, {1}, {0})", lrp, 4, {1, std::min<uint8_t>(4, lrp.dst.swizzle.size()),
																			std::min<uint8_t>(4, lrp.dst.swizzle.size())});
					},
				[](const Frc &frc) { return formatInstruction("fract({})", frc); },
				[](const Pow &pow) { return formatInstruction("pow({}, {})", pow); },
				[](const Abs &abs) { return formatInstruction("abs({})", abs); },
				[](const Nrm &nrm) { return formatInstruction("normalize({})", nrm); },
				[](const SinCos &sincos) { return formatInstruction("vec2(cos({0}), sin({0}))", sincos, 2, 1); },
				[&](const Ifc &ifc) { indent++; return formatInstruction("if({} == {}) {{\n", ifc); },
				[&](const Else &) -> std::string { return "} else {\n"; },
				[&](const EndIf &) -> std::string { indent--; return "}\n"; },
				[](const Tex &tex) {
					unsigned int l = 0;
					switch (*tex.src[1].samplerType) {
						case Type::kTexture2D:
							l = 2;
							break;

						case Type::kTexture3D:
						case Type::kTextureCube:
							l = 3;
							break;

						default:
							throw CreateException("Invalid sampler type");
					}

					return formatInstruction("texture({1}, {0})", tex, 4, l);
				},
				[](const Cmp &cmp) {
					return formatInstruction(
							"({} >= 0) ? {} : {}", cmp, 4,
							{
								1,
								std::min<uint8_t>(4, cmp.dst.swizzle.size()),
								std::min<uint8_t>(4, cmp.dst.swizzle.size())
							}
					);
				},
				[](const Dp2Add &dp2add) { return formatInstruction("dot({}, {}) + {}", dp2add, 1, {2, 2, 1}); },
		}, instruction);
	}

	source += "}\n";

	return source;
}

std::string DXBC::formatRegisterName(RegisterType type, unsigned int number) {
	std::string name;
	switch (type) {
		case kConst: {
			const auto uniformIter = _uniformNames.find(number);
			if (uniformIter == _uniformNames.end())
				name = std::format("c{}", number);
			else
				name = uniformIter->second;
			break;
		}

		case kInput: {
			const auto attributeIter = _attributeInNames.find(number);
			if (attributeIter == _attributeInNames.end())
				name = std::format("in_{}", number);
			else
				name = attributeIter->second;
			break;
		}

		case kColorOut: {
			const auto uniformIter = _uniformTextureNames.find(number);
			if (uniformIter == _uniformTextureNames.end())
				name = std::format("s{}", number);
			else
				name = uniformIter->second;
			break;
		}

		case kAttrOut:
			[[fallthrough]];
		case kOutput: {
			const auto attributeIter = _attributeOutNames.find(number);
			if (attributeIter == _attributeOutNames.end())
				name = std::format("out_{}", number);
			else
				name = attributeIter->second;
			break;
		}

		case kLoop:
		case kTemp:
			name = std::format("t{}", number);
			_tempVariables.emplace(number);
			break;

		default:
			throw CreateException("Unimplemented register type {}", fmt::underlying(type));
	}

	return name;
}

Source DXBC::readSource(ReadStream &dxbc) {
	Common::BitStream8LSB bits(dxbc);

	const auto registerNumber = bits.read(11);
	const auto registerType1 = bits.read(2);
	const auto relativeAddressing = bits.read();
	bits.skip(2);

	assert(!relativeAddressing);

	const auto swizzleId = bits.read(8);
	const auto sourceModifier = SourceModifier(bits.read(4));

	const auto registerType2 = bits.read(3);
	bits.skip(1);

	const auto registerType = RegisterType((registerType1 << 3) | registerType2);

	std::string swizzle;
	for (unsigned int i = 0; i < 4; ++i) {
		const auto s = (swizzleId >> (i * 2)) & 3;

		switch (s) {
			case 0:
				swizzle += "x";
				break;
			case 1:
				swizzle += "y";
				break;
			case 2:
				swizzle += "z";
				break;
			case 3:
				swizzle += "w";
				break;
			default:
				break;
		}
	}

	const auto name = formatRegisterName(registerType, registerNumber);
	std::optional<Type> samplerType;

	for (const auto &uniform: _uniforms) {
		if (uniform.name != name)
			continue;

		if (uniform.type == Type::kFloat)
			swizzle = "";

		if (uniform.type == Type::kTexture2D || uniform.type == Type::kTexture3D || uniform.type == Type::kTextureCube)
			samplerType = uniform.type;
	}

	if (registerType == kTexture || registerType == kColorOut)
		swizzle = "";

	return Source{
			name,
			swizzle,
			registerType,
			sourceModifier,
			samplerType
	};
}

Destination DXBC::readDestination(ReadStream &dxbc) {
	Common::BitStream8LSB bits(dxbc);

	const auto registerNumber = bits.read(11);
	const auto registerType1 = bits.read(2);
	const auto relativeAddressing = bits.read();
	bits.skip(2);

	const auto writeMask = bits.read(4);
	const auto resultModifier = bits.read(4);

	bits.skip(4);
	const auto registerType2 = bits.read(3);

	bits.skip(1);

	std::string swizzle;
	if (writeMask & 1)
		swizzle += "x";
	if (writeMask & 2)
		swizzle += "y";
	if (writeMask & 4)
		swizzle += "z";
	if (writeMask & 8)
		swizzle += "w";

	const auto registerType = RegisterType((registerType1 << 3) | registerType2);

	const auto name = formatRegisterName(registerType, registerNumber);

	if (registerType == kOutput && _attributeOutNames.find(registerNumber) == _attributeOutNames.end())
		_attributeOutNames[registerNumber] = name;

	return Destination{
			registerNumber,
			name,
			swizzle,
			registerType,
			(resultModifier & 1) != 0,
			(resultModifier & 2) != 0,
			(resultModifier & 4) != 0,
			{writeMask}
	};
}

void DXBC::readAttribute(ReadStream &dxbc) {
	Common::BitStream8LSB bits(dxbc);

	// Attribute info
	const auto usage = Usage(bits.read(5));
	bits.skip(11);
	const auto usageIndex = bits.read(4);
	bits.skip(11);

	const auto test = bits.read();
	assert(test);

	// Destination token info
	const auto dst = readDestination(dxbc);


	Direction dir;
	switch (dst.type) {
		case kLoop:
		case kInput:
			dir = _in;
			break;
		case kOutput:
			[[fallthrough]];
		case kColorOut:
			[[fallthrough]];
		case kAttrOut:
			dir = _out;
			break;
		default:
			throw CreateException("Invalid destination token, expected input or output token");
	}

	const auto decName = formatDeclarationName(usage, dir, usageIndex);
	const auto dstName = formatRegisterName(dst.type, dst.id);

	switch (dst.type) {
		case kInput:
			_attributeInNames[dst.id] = decName;
			break;

		case kOutput:
			[[fallthrough]];
		case kAttrOut: //[[fallthrough]];
			_attributeOutNames[dst.id] = decName;
			break;

		default:
			break;
	}
}

void DXBC::readConstants(Common::ReadStream &dxbc) {
	const auto dstName = readDestination(dxbc).name;
	const auto val = dxbc.read<glm::vec4>();

	_constants.emplace_back(dstName, val);
}

} // Graphics