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

#include "src/graphics/gfxman.h"
#include "src/graphics/font_binfnt.h"
#include "src/graphics/textureman.h"
#include "src/graphics/images/dds.h"

namespace Graphics {

BINFNTFont::BINFNTFont(Common::ReadStream &binfnt) {
	uint32_t version = binfnt.readUint32LE();
	if ((version != 3) && (version != 7))
		throw std::runtime_error("Invalid binfnt version");

	uint32_t numVertices = binfnt.readUint32LE();
	Common::ByteBuffer verticesData(numVertices * 4 * 4);
	binfnt.read(verticesData.data(), numVertices * 4 * 4);

	uint32_t numIndices = binfnt.readUint32LE();
	Common::ByteBuffer indicesData(numIndices * 2);
	binfnt.read(indicesData.data(), numIndices * 2);

	_vertices = GfxMan.createBuffer(std::move(verticesData), kVertexBuffer, false);
	_indices = GfxMan.createBuffer(std::move(indicesData), kIndexBuffer, false);

	std::vector<VertexAttribute> attributes = {
			{kPosition,  kVec2F},
			{kTexCoord0, kVec2F}
	};
	_vao = GfxMan.createAttributeObject(
		"gui",
		"material",
		0,
		attributes,
		_vertices
	);

	const uint32_t numGlyphs = binfnt.readUint32LE();
	std::queue<Glyph> glyphs;
	for (unsigned int i = 0; i < numGlyphs; ++i) {
		Glyph glyph;
		glyph.vertexOffset = binfnt.readUint16LE();
		glyph.vertexCount = binfnt.readUint16LE();
		glyph.indexOffset = binfnt.readUint16LE();
		glyph.indexCount = binfnt.readUint16LE();
		glyph.glyphWidth = 0.0f;

		// Unknown values
		binfnt.skip(36);

		glyphs.push(glyph);
	}

	for (int i = 0; i < USHRT_MAX + 1; ++i) {
		char16_t code = binfnt.readUint16LE() + 0x20;
		if (_glyphs.find(code) != _glyphs.end())
			continue;

		_glyphs[code] = glyphs.front();
		glyphs.pop();
	}

	// version 7 has kern information
	if (version == 7) {
		const uint32_t numKerns = binfnt.readUint32LE();
		for (unsigned int i = 0; i < numKerns; ++i) {
			binfnt.skip(8); // Ignore Kerning info
		}
	}

	std::unique_ptr<Common::ReadStream> textureStream;
	if (version == 3) {
		uint32_t textureSize = binfnt.readUint32LE();
		textureStream = std::unique_ptr<Common::ReadStream>(binfnt.readStream(textureSize));
	} else if (version == 7) {
		binfnt.skip(8); // Always 0
		textureStream = std::unique_ptr<Common::ReadStream>(binfnt.readStream(binfnt.size() - binfnt.pos()));
	}

	DDS dds(textureStream.get());
	_texture = GfxMan.createTexture(std::move(dds));
}

BINFNTFont::~BINFNTFont() {
}

void BINFNTFont::getCharacterValues(char16_t c, uint16_t &vertexOffset, uint16_t &indexOffset, uint16_t &indexCount,
									float &characterWidth) {
	Glyph g = _glyphs[c];
	vertexOffset = g.vertexOffset;
	indexOffset = g.indexOffset;
	indexCount = g.indexCount;
}

}
