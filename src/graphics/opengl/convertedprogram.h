//
// Created by patrick on 27.12.21.
//

#ifndef OPENAWE_CONVERTEDPROGRAM_H
#define OPENAWE_CONVERTEDPROGRAM_H

#include "src/graphics/opengl/program.h"

namespace Graphics::OpenGL {

class ConvertedProgram : public Program {
public:
	void setAttributeMappings(const std::map<AttributeType, std::string> &mappings);
	void addSamplerMappings(const std::map<std::string, std::string> &mappings);

	void setSymbols(const std::vector<ShaderConverter::Symbol> &symbols);

	std::optional<GLint> getAttributeLocation(const AttributeType &type) override;

	std::optional<GLint> getUniformLocation(const std::string &name) const override;

	void setUniform1f(GLint id, const glm::vec1 &value) const override;

	void setUniform2f(GLint id, const glm::vec2 &value) const override;

	void setUniform3f(GLint id, const glm::vec3 &value) const override;

	void setUniformMatrix4f(GLint id, const glm::mat4 &value) const override;

private:
	std::optional<GLint> getUniformArraySymbolLocation(const ShaderConverter::Symbol &symbol, unsigned int offset = 0) const;

	std::map<std::string, std::string> _samplerMappings;
	std::map<AttributeType, std::string> _attributeMappings;
	std::map<std::string, ShaderConverter::Symbol> _symbols;
};

} // End of namespace Graphics::OpenGL

#endif //OPENAWE_CONVERTEDPROGRAM_H
