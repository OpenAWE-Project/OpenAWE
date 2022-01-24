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

#include <GLFW/glfw3.h>

#include "src/platform/keyconversion.h"

namespace Platform {

Events::Key convertGLFW2Key(int key) {
	switch (key) {
		case GLFW_KEY_SPACE:         return Events::kKeySpace;
		case GLFW_KEY_APOSTROPHE:    return Events::kKeyApostrophe;
		case GLFW_KEY_COMMA:         return Events::kKeyComma;
		case GLFW_KEY_MINUS:         return Events::kKeyMinus;
		case GLFW_KEY_PERIOD:        return Events::kKeyPeriod;
		case GLFW_KEY_SLASH:         return Events::kKeySlash;

		case GLFW_KEY_0:             return Events::kKey0;
		case GLFW_KEY_1:             return Events::kKey1;
		case GLFW_KEY_2:             return Events::kKey2;
		case GLFW_KEY_3:             return Events::kKey3;
		case GLFW_KEY_4:             return Events::kKey4;
		case GLFW_KEY_5:             return Events::kKey5;
		case GLFW_KEY_6:             return Events::kKey6;
		case GLFW_KEY_7:             return Events::kKey7;
		case GLFW_KEY_8:             return Events::kKey8;
		case GLFW_KEY_9:             return Events::kKey9;

		case GLFW_KEY_SEMICOLON:     return Events::kKeySemicolon;
		case GLFW_KEY_EQUAL:         return Events::kKeyEqual;

		case GLFW_KEY_A:             return Events::kKeyA;
		case GLFW_KEY_B:             return Events::kKeyB;
		case GLFW_KEY_C:             return Events::kKeyC;
		case GLFW_KEY_D:             return Events::kKeyD;
		case GLFW_KEY_E:             return Events::kKeyE;
		case GLFW_KEY_F:             return Events::kKeyF;
		case GLFW_KEY_G:             return Events::kKeyG;
		case GLFW_KEY_H:             return Events::kKeyH;
		case GLFW_KEY_I:             return Events::kKeyI;
		case GLFW_KEY_J:             return Events::kKeyJ;
		case GLFW_KEY_K:             return Events::kKeyK;
		case GLFW_KEY_L:             return Events::kKeyL;
		case GLFW_KEY_M:             return Events::kKeyM;
		case GLFW_KEY_N:             return Events::kKeyN;
		case GLFW_KEY_O:             return Events::kKeyO;
		case GLFW_KEY_P:             return Events::kKeyP;
		case GLFW_KEY_Q:             return Events::kKeyQ;
		case GLFW_KEY_R:             return Events::kKeyR;
		case GLFW_KEY_S:             return Events::kKeyS;
		case GLFW_KEY_T:             return Events::kKeyT;
		case GLFW_KEY_U:             return Events::kKeyU;
		case GLFW_KEY_V:             return Events::kKeyV;
		case GLFW_KEY_W:             return Events::kKeyW;
		case GLFW_KEY_X:             return Events::kKeyX;
		case GLFW_KEY_Y:             return Events::kKeyY;
		case GLFW_KEY_Z:             return Events::kKeyZ;

		case GLFW_KEY_LEFT_BRACKET:  return Events::kKeyLeftBracket;
		case GLFW_KEY_BACKSLASH:     return Events::kKeyBackslash;
		case GLFW_KEY_RIGHT_BRACKET: return Events::kKeyRightBracket;
		case GLFW_KEY_GRAVE_ACCENT:  return Events::kKeyGraveAccent;

		case GLFW_KEY_WORLD_1:       return Events::kKeyWorld1;
		case GLFW_KEY_WORLD_2:       return Events::kKeyWorld2;

		case GLFW_KEY_ESCAPE:        return Events::kKeyEscape;
		case GLFW_KEY_ENTER:         return Events::kKeyEnter;
		case GLFW_KEY_TAB:           return Events::kKeyTab;
		case GLFW_KEY_BACKSPACE:     return Events::kKeyBackspace;

		case GLFW_KEY_INSERT:        return Events::kKeyInsert;
		case GLFW_KEY_DELETE:        return Events::kKeyDelete;

		case GLFW_KEY_RIGHT:         return Events::kKeyRight;
		case GLFW_KEY_LEFT:          return Events::kKeyLeft;
		case GLFW_KEY_DOWN:          return Events::kKeyDown;
		case GLFW_KEY_UP:            return Events::kKeyUp;

		case GLFW_KEY_PAGE_UP:       return Events::kKeyPageUp;
		case GLFW_KEY_PAGE_DOWN:     return Events::kKeyPageDown;

		case GLFW_KEY_HOME:          return Events::kKeyHome;
		case GLFW_KEY_END:           return Events::kKeyEnd;
		case GLFW_KEY_CAPS_LOCK:     return Events::kKeyCapsLock;
		case GLFW_KEY_SCROLL_LOCK:   return Events::kKeyScrollLock;
		case GLFW_KEY_NUM_LOCK:      return Events::kKeyNumLock;
		case GLFW_KEY_PRINT_SCREEN:  return Events::kKeyPrintScreen;
		case GLFW_KEY_PAUSE:         return Events::kKeyPause;

		case GLFW_KEY_F1:            return Events::kKeyF1;
		case GLFW_KEY_F2:            return Events::kKeyF2;
		case GLFW_KEY_F3:            return Events::kKeyF3;
		case GLFW_KEY_F4:            return Events::kKeyF4;
		case GLFW_KEY_F5:            return Events::kKeyF5;
		case GLFW_KEY_F6:            return Events::kKeyF6;
		case GLFW_KEY_F7:            return Events::kKeyF7;
		case GLFW_KEY_F8:            return Events::kKeyF8;
		case GLFW_KEY_F9:            return Events::kKeyF9;
		case GLFW_KEY_F11:           return Events::kKeyF11;
		case GLFW_KEY_F12:           return Events::kKeyF12;
		case GLFW_KEY_F13:           return Events::kKeyF13;
		case GLFW_KEY_F14:           return Events::kKeyF14;
		case GLFW_KEY_F15:           return Events::kKeyF15;
		case GLFW_KEY_F16:           return Events::kKeyF16;
		case GLFW_KEY_F17:           return Events::kKeyF17;
		case GLFW_KEY_F18:           return Events::kKeyF18;
		case GLFW_KEY_F19:           return Events::kKeyF19;
		case GLFW_KEY_F20:           return Events::kKeyF20;
		case GLFW_KEY_F21:           return Events::kKeyF21;
		case GLFW_KEY_F22:           return Events::kKeyF22;
		case GLFW_KEY_F23:           return Events::kKeyF23;
		case GLFW_KEY_F24:           return Events::kKeyF24;

		case GLFW_KEY_KP_0:          return Events::kKeyKeypad0;
		case GLFW_KEY_KP_1:          return Events::kKeyKeypad1;
		case GLFW_KEY_KP_2:          return Events::kKeyKeypad2;
		case GLFW_KEY_KP_3:          return Events::kKeyKeypad3;
		case GLFW_KEY_KP_4:          return Events::kKeyKeypad4;
		case GLFW_KEY_KP_5:          return Events::kKeyKeypad5;
		case GLFW_KEY_KP_6:          return Events::kKeyKeypad6;
		case GLFW_KEY_KP_7:          return Events::kKeyKeypad7;
		case GLFW_KEY_KP_8:          return Events::kKeyKeypad8;
		case GLFW_KEY_KP_9:          return Events::kKeyKeypad9;

		case GLFW_KEY_KP_DECIMAL:    return Events::kKeyKeypadDecimal;
		case GLFW_KEY_KP_DIVIDE:     return Events::kKeyKeypadDivide;
		case GLFW_KEY_KP_MULTIPLY:   return Events::kKeyKeypadMultiply;
		case GLFW_KEY_KP_SUBTRACT:   return Events::kKeyKeypadSubtract;
		case GLFW_KEY_KP_ADD:        return Events::kKeyKeypadAdd;
		case GLFW_KEY_KP_ENTER:      return Events::kKeyKeypadEnter;
		case GLFW_KEY_KP_EQUAL:      return Events::kKeyKeypadEqual;

		case GLFW_KEY_LEFT_SHIFT:    return Events::kKeyLeftShift;
		case GLFW_KEY_LEFT_ALT:      return Events::kKeyLeftAlt;
		case GLFW_KEY_LEFT_CONTROL:  return Events::kKeyLeftControl;
		case GLFW_KEY_LEFT_SUPER:    return Events::kKeyLeftSuper;

		case GLFW_KEY_RIGHT_SHIFT:    return Events::kKeyRightShift;
		case GLFW_KEY_RIGHT_ALT:      return Events::kKeyRightAlt;
		case GLFW_KEY_RIGHT_CONTROL:  return Events::kKeyRightControl;
		case GLFW_KEY_RIGHT_SUPER:    return Events::kKeyRightSuper;

		case GLFW_KEY_MENU:           return Events::kKeyMenu;

		case GLFW_KEY_UNKNOWN:       [[fallthrough]];
		default:
			return Events::kKeyUnknown;
	}
}

} // End of namespace Platform
