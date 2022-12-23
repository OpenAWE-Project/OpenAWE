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

#include <codecvt>
#include <locale>

#include "src/graphics/text.h"
#include "src/graphics/fontman.h"

namespace Graphics {

Text::Text() : _font(FontMan.get("fixedsys")), _text(u"") {
	_vao = _font.getVAO();
	rebuildText();
}

void Text::setText(const std::string &text) {
	_text = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(text);
	rebuildText();
}

void Text::setText(const std::u16string &text) {
	_text = text;
	rebuildText();
}

void Text::rebuildText() {
	_parts.clear();
	float offset = 0.0f;
	for (const auto &c : _text) {
		GUIElementPart part;
		float glyphWidth;
		_font.getCharacterValues(c, part.verticesOffset, part.indicesOffset, part.indicesCount, glyphWidth);
		part.texture = _font.getTexture();
		part.vertices = _font.getVertices();
		part.indices = _font.getIndices();
		part.position.x = offset;
		part.position.y = 0;
		offset += 1.0f;

		part.relativeSize = glm::vec2(0.0, 0.0);
		part.absoluteSize = glm::vec2(16, 16);

		_parts.emplace_back(part);
	}
}

}
