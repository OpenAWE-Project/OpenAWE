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

#ifndef OPENAWE_KEY_H
#define OPENAWE_KEY_H

namespace Events {

/*!
 * An enum representing keyboard keys. The list is mirrored with the key macros from glfw.
 */
enum Key {
	kKeyUnknown,

	kKeySpace,
	kKeyApostrophe,
	kKeyComma,
	kKeyMinus,
	kKeyPeriod,
	kKeySlash,

	kKey0,
	kKey1,
	kKey2,
	kKey3,
	kKey4,
	kKey5,
	kKey6,
	kKey7,
	kKey8,
	kKey9,

	kKeySemicolon,
	kKeyEqual,

	kKeyA,
	kKeyB,
	kKeyC,
	kKeyD,
	kKeyE,
	kKeyF,
	kKeyG,
	kKeyH,
	kKeyI,
	kKeyJ,
	kKeyK,
	kKeyL,
	kKeyM,
	kKeyN,
	kKeyO,
	kKeyP,
	kKeyQ,
	kKeyR,
	kKeyS,
	kKeyT,
	kKeyU,
	kKeyV,
	kKeyW,
	kKeyX,
	kKeyY,
	kKeyZ,

	kKeyLeftBracket,
	kKeyBackslash,
	kKeyRightBracket,
	kKeyGraveAccent,
	kKeyWorld1,
	kKeyWorld2,

	kKeyEscape,
	kKeyEnter,
	kKeyTab,
	kKeyBackspace,

	kKeyInsert,
	kKeyDelete,

	kKeyRight,
	kKeyLeft,
	kKeyDown,
	kKeyUp,

	kKeyPageUp,
	kKeyPageDown,

	kKeyHome,
	kKeyEnd,
	kKeyCapsLock,
	kKeyScrollLock,
	kKeyNumLock,
	kKeyPrintScreen,
	kKeyPause,

	kKeyF1,
	kKeyF2,
	kKeyF3,
	kKeyF4,
	kKeyF5,
	kKeyF6,
	kKeyF7,
	kKeyF8,
	kKeyF9,
	kKeyF10,
	kKeyF11,
	kKeyF12,
	kKeyF13,
	kKeyF14,
	kKeyF15,
	kKeyF16,
	kKeyF17,
	kKeyF18,
	kKeyF19,
	kKeyF20,
	kKeyF21,
	kKeyF22,
	kKeyF23,
	kKeyF24,

	kKeyKeypad0,
	kKeyKeypad1,
	kKeyKeypad2,
	kKeyKeypad3,
	kKeyKeypad4,
	kKeyKeypad5,
	kKeyKeypad6,
	kKeyKeypad7,
	kKeyKeypad8,
	kKeyKeypad9,

	kKeyKeypadDecimal,
	kKeyKeypadDivide,
	kKeyKeypadMultiply,
	kKeyKeypadSubtract,
	kKeyKeypadAdd,
	kKeyKeypadEnter,
	kKeyKeypadEqual,
	kKeyLeftShift,
	kKeyLeftControl,
	kKeyLeftAlt,
	kKeyLeftSuper,
	kKeyRightShift,
	kKeyRightControl,
	kKeyRightAlt,
	kKeyRightSuper,
	kKeyMenu
};


/*!
 * An enum representing keyboard key modifiers. 
 * The list is mirrored with the modifier macros from glfw,
 * with the addition of kNoModifier for key combination handling.
 */
enum KeyModifier {
	kNoModifier = 0x0,
	kModifierShift = 0x1,
	kModifierControl = 0x2,
	kModifierAlt = 0x4,
	kModifierSuper = 0x8,
	kModifierCapsLock = 0x10,
	kModifierNumLock = 0x20
};

} // End of namespace Key

#endif //OPENAWE_KEY_H
