/*
 * Copyright 2023 VeryGoodGraphics LTD <bd@verygoodgraphics.com>
 *
 * Licensed under the VGG License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     https://www.verygoodgraphics.com/licenses/LICENSE-1.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "Event/Keycode.hpp"
#include "Utility/Log.hpp"
#include "Event/EventAPI.hpp"

static std::unique_ptr<EventAPI> g_vggEventAPIImpl = nullptr;

void EventManager::registerEventAPI(std::unique_ptr<EventAPI> impl)
{
  g_vggEventAPIImpl = std::move(impl);
}

EVGGKeymod EventManager::getModState()
{
  ASSERT_MSG(g_vggEventAPIImpl, "Event API is not initialized");
  return g_vggEventAPIImpl->getModState();
}

uint8_t* EventManager::getKeyboardState(int* nums)
{
  ASSERT_MSG(g_vggEventAPIImpl, "Event API is not initialized");
  return g_vggEventAPIImpl->getKeyboardState(nums);
}

EVGGKeyCode EventManager::getKeyFromScancode(EVGGScancode scancode)
{
  if (((int)scancode) < VGG_SCANCODE_UNKNOWN || scancode >= VGG_NUM_SCANCODES)
  {
    return VGGK_UNKNOWN;
  }

  static const int s_keyMap[VGG_NUM_SCANCODES] = {
    /* 0 */ 0,
    /* 1 */ 0,
    /* 2 */ 0,
    /* 3 */ 0,
    /* 4 */ 'a',
    /* 5 */ 'b',
    /* 6 */ 'c',
    /* 7 */ 'd',
    /* 8 */ 'e',
    /* 9 */ 'f',
    /* 10 */ 'g',
    /* 11 */ 'h',
    /* 12 */ 'i',
    /* 13 */ 'j',
    /* 14 */ 'k',
    /* 15 */ 'l',
    /* 16 */ 'm',
    /* 17 */ 'n',
    /* 18 */ 'o',
    /* 19 */ 'p',
    /* 20 */ 'q',
    /* 21 */ 'r',
    /* 22 */ 's',
    /* 23 */ 't',
    /* 24 */ 'u',
    /* 25 */ 'v',
    /* 26 */ 'w',
    /* 27 */ 'x',
    /* 28 */ 'y',
    /* 29 */ 'z',
    /* 30 */ '1',
    /* 31 */ '2',
    /* 32 */ '3',
    /* 33 */ '4',
    /* 34 */ '5',
    /* 35 */ '6',
    /* 36 */ '7',
    /* 37 */ '8',
    /* 38 */ '9',
    /* 39 */ '0',
    /* 40 */ VGG_SCANCODE_RETURN,
    /* 41 */ VGG_SCANCODE_ESCAPE,
    /* 42 */ VGG_SCANCODE_BACKSPACE,
    /* 43 */ VGG_SCANCODE_TAB,
    /* 44 */ VGG_SCANCODE_SPACE,
    /* 45 */ '-',
    /* 46 */ '=',
    /* 47 */ '[',
    /* 48 */ ']',
    /* 49 */ '\\',
    /* 50 */ '#',
    /* 51 */ ';',
    /* 52 */ '\'',
    /* 53 */ '`',
    /* 54 */ ',',
    /* 55 */ '.',
    /* 56 */ '/',
    /* 57 */ VGG_SCANCODE_CAPSLOCK,
    /* 58 */ VGG_SCANCODE_F1,
    /* 59 */ VGG_SCANCODE_F2,
    /* 60 */ VGG_SCANCODE_F3,
    /* 61 */ VGG_SCANCODE_F4,
    /* 62 */ VGG_SCANCODE_F5,
    /* 63 */ VGG_SCANCODE_F6,
    /* 64 */ VGG_SCANCODE_F7,
    /* 65 */ VGG_SCANCODE_F8,
    /* 66 */ VGG_SCANCODE_F9,
    /* 67 */ VGG_SCANCODE_F10,
    /* 68 */ VGG_SCANCODE_F11,
    /* 69 */ VGG_SCANCODE_F12,
    /* 70 */ VGG_SCANCODE_PRINTSCREEN,
    /* 71 */ VGG_SCANCODE_SCROLLLOCK,
    /* 72 */ VGG_SCANCODE_PAUSE,
    /* 73 */ VGG_SCANCODE_INSERT,
    /* 74 */ VGG_SCANCODE_HOME,
    /* 75 */ VGG_SCANCODE_PAGEUP,
    /* 76 */ VGG_SCANCODE_DELETE,
    /* 77 */ VGG_SCANCODE_END,
    /* 78 */ VGG_SCANCODE_PAGEDOWN,
    /* 79 */ VGG_SCANCODE_RIGHT,
    /* 80 */ VGG_SCANCODE_LEFT,
    /* 81 */ VGG_SCANCODE_DOWN,
    /* 82 */ VGG_SCANCODE_UP,
    /* 83 */ VGG_SCANCODE_NUMLOCKCLEAR,
    /* 84 */ VGG_SCANCODE_KP_DIVIDE,
    /* 85 */ VGG_SCANCODE_KP_MULTIPLY,
    /* 86 */ VGG_SCANCODE_KP_MINUS,
    /* 87 */ VGG_SCANCODE_KP_PLUS,
    /* 88 */ VGG_SCANCODE_KP_ENTER,
    /* 89 */ VGG_SCANCODE_KP_1,
    /* 90 */ VGG_SCANCODE_KP_2,
    /* 91 */ VGG_SCANCODE_KP_3,
    /* 92 */ VGG_SCANCODE_KP_4,
    /* 93 */ VGG_SCANCODE_KP_5,
    /* 94 */ VGG_SCANCODE_KP_6,
    /* 95 */ VGG_SCANCODE_KP_7,
    /* 96 */ VGG_SCANCODE_KP_8,
    /* 97 */ VGG_SCANCODE_KP_9,
    /* 98 */ VGG_SCANCODE_KP_0,
    /* 99 */ VGG_SCANCODE_KP_PERIOD,
    /* 100 */ 0,
    /* 101 */ VGG_SCANCODE_APPLICATION,
    /* 102 */ VGG_SCANCODE_POWER,
    /* 103 */ VGG_SCANCODE_KP_EQUALS,
    /* 104 */ VGG_SCANCODE_F13,
    /* 105 */ VGG_SCANCODE_F14,
    /* 106 */ VGG_SCANCODE_F15,
    /* 107 */ VGG_SCANCODE_F16,
    /* 108 */ VGG_SCANCODE_F17,
    /* 109 */ VGG_SCANCODE_F18,
    /* 110 */ VGG_SCANCODE_F19,
    /* 111 */ VGG_SCANCODE_F20,
    /* 112 */ VGG_SCANCODE_F21,
    /* 113 */ VGG_SCANCODE_F22,
    /* 114 */ VGG_SCANCODE_F23,
    /* 115 */ VGG_SCANCODE_F24,
    /* 116 */ VGG_SCANCODE_EXECUTE,
    /* 117 */ VGG_SCANCODE_HELP,
    /* 118 */ VGG_SCANCODE_MENU,
    /* 119 */ VGG_SCANCODE_SELECT,
    /* 120 */ VGG_SCANCODE_STOP,
    /* 121 */ VGG_SCANCODE_AGAIN,
    /* 122 */ VGG_SCANCODE_UNDO,
    /* 123 */ VGG_SCANCODE_CUT,
    /* 124 */ VGG_SCANCODE_COPY,
    /* 125 */ VGG_SCANCODE_PASTE,
    /* 126 */ VGG_SCANCODE_FIND,
    /* 127 */ VGG_SCANCODE_MUTE,
    /* 128 */ VGG_SCANCODE_VOLUMEUP,
    /* 129 */ VGG_SCANCODE_VOLUMEDOWN,
    /* 130 */ 0,
    /* 131 */ 0,
    /* 132 */ 0,
    /* 133 */ VGG_SCANCODE_KP_COMMA,
    /* 134 */ VGG_SCANCODE_KP_EQUALSAS400,
    /* 135 */ 0,
    /* 136 */ 0,
    /* 137 */ 0,
    /* 138 */ 0,
    /* 139 */ 0,
    /* 140 */ 0,
    /* 141 */ 0,
    /* 142 */ 0,
    /* 143 */ 0,
    /* 144 */ 0,
    /* 145 */ 0,
    /* 146 */ 0,
    /* 147 */ 0,
    /* 148 */ 0,
    /* 149 */ 0,
    /* 150 */ 0,
    /* 151 */ 0,
    /* 152 */ 0,
    /* 153 */ VGG_SCANCODE_ALTERASE,
    /* 154 */ VGG_SCANCODE_SYSREQ,
    /* 155 */ VGG_SCANCODE_CANCEL,
    /* 156 */ VGG_SCANCODE_CLEAR,
    /* 157 */ VGG_SCANCODE_PRIOR,
    /* 158 */ VGG_SCANCODE_RETURN2,
    /* 159 */ VGG_SCANCODE_SEPARATOR,
    /* 160 */ VGG_SCANCODE_OUT,
    /* 161 */ VGG_SCANCODE_OPER,
    /* 162 */ VGG_SCANCODE_CLEARAGAIN,
    /* 163 */ VGG_SCANCODE_CRSEL,
    /* 164 */ VGG_SCANCODE_EXSEL,
    /* 165 */ 0,
    /* 166 */ 0,
    /* 167 */ 0,
    /* 168 */ 0,
    /* 169 */ 0,
    /* 170 */ 0,
    /* 171 */ 0,
    /* 172 */ 0,
    /* 173 */ 0,
    /* 174 */ 0,
    /* 175 */ 0,
    /* 176 */ VGG_SCANCODE_KP_00,
    /* 177 */ VGG_SCANCODE_KP_000,
    /* 178 */ VGG_SCANCODE_THOUSANDSSEPARATOR,
    /* 179 */ VGG_SCANCODE_DECIMALSEPARATOR,
    /* 180 */ VGG_SCANCODE_CURRENCYUNIT,
    /* 181 */ VGG_SCANCODE_CURRENCYSUBUNIT,
    /* 182 */ VGG_SCANCODE_KP_LEFTPAREN,
    /* 183 */ VGG_SCANCODE_KP_RIGHTPAREN,
    /* 184 */ VGG_SCANCODE_KP_LEFTBRACE,
    /* 185 */ VGG_SCANCODE_KP_RIGHTBRACE,
    /* 186 */ VGG_SCANCODE_KP_TAB,
    /* 187 */ VGG_SCANCODE_KP_BACKSPACE,
    /* 188 */ VGG_SCANCODE_KP_A,
    /* 189 */ VGG_SCANCODE_KP_B,
    /* 190 */ VGG_SCANCODE_KP_C,
    /* 191 */ VGG_SCANCODE_KP_D,
    /* 192 */ VGG_SCANCODE_KP_E,
    /* 193 */ VGG_SCANCODE_KP_F,
    /* 194 */ VGG_SCANCODE_KP_XOR,
    /* 195 */ VGG_SCANCODE_KP_POWER,
    /* 196 */ VGG_SCANCODE_KP_PERCENT,
    /* 197 */ VGG_SCANCODE_KP_LESS,
    /* 198 */ VGG_SCANCODE_KP_GREATER,
    /* 199 */ VGG_SCANCODE_KP_AMPERSAND,
    /* 200 */ VGG_SCANCODE_KP_DBLAMPERSAND,
    /* 201 */ VGG_SCANCODE_KP_VERTICALBAR,
    /* 202 */ VGG_SCANCODE_KP_DBLVERTICALBAR,
    /* 203 */ VGG_SCANCODE_KP_COLON,
    /* 204 */ VGG_SCANCODE_KP_HASH,
    /* 205 */ VGG_SCANCODE_KP_SPACE,
    /* 206 */ VGG_SCANCODE_KP_AT,
    /* 207 */ VGG_SCANCODE_KP_EXCLAM,
    /* 208 */ VGG_SCANCODE_KP_MEMSTORE,
    /* 209 */ VGG_SCANCODE_KP_MEMRECALL,
    /* 210 */ VGG_SCANCODE_KP_MEMCLEAR,
    /* 211 */ VGG_SCANCODE_KP_MEMADD,
    /* 212 */ VGG_SCANCODE_KP_MEMSUBTRACT,
    /* 213 */ VGG_SCANCODE_KP_MEMMULTIPLY,
    /* 214 */ VGG_SCANCODE_KP_MEMDIVIDE,
    /* 215 */ VGG_SCANCODE_KP_PLUSMINUS,
    /* 216 */ VGG_SCANCODE_KP_CLEAR,
    /* 217 */ VGG_SCANCODE_KP_CLEARENTRY,
    /* 218 */ VGG_SCANCODE_KP_BINARY,
    /* 219 */ VGG_SCANCODE_KP_OCTAL,
    /* 220 */ VGG_SCANCODE_KP_DECIMAL,
    /* 221 */ VGG_SCANCODE_KP_HEXADECIMAL,
    /* 222 */ 0,
    /* 223 */ 0,
    /* 224 */ VGG_SCANCODE_LCTRL,
    /* 225 */ VGG_SCANCODE_LSHIFT,
    /* 226 */ VGG_SCANCODE_LALT,
    /* 227 */ VGG_SCANCODE_LGUI,
    /* 228 */ VGG_SCANCODE_RCTRL,
    /* 229 */ VGG_SCANCODE_RSHIFT,
    /* 230 */ VGG_SCANCODE_RALT,
    /* 231 */ VGG_SCANCODE_RGUI,
    /* 232 */ 0,
    /* 233 */ 0,
    /* 234 */ 0,
    /* 235 */ 0,
    /* 236 */ 0,
    /* 237 */ 0,
    /* 238 */ 0,
    /* 239 */ 0,
    /* 240 */ 0,
    /* 241 */ 0,
    /* 242 */ 0,
    /* 243 */ 0,
    /* 244 */ 0,
    /* 245 */ 0,
    /* 246 */ 0,
    /* 247 */ 0,
    /* 248 */ 0,
    /* 249 */ 0,
    /* 250 */ 0,
    /* 251 */ 0,
    /* 252 */ 0,
    /* 253 */ 0,
    /* 254 */ 0,
    /* 255 */ 0,
    /* 256 */ 0,
    /* 257 */ VGG_SCANCODE_MODE,
    /* 258 */ VGG_SCANCODE_AUDIONEXT,
    /* 259 */ VGG_SCANCODE_AUDIOPREV,
    /* 260 */ VGG_SCANCODE_AUDIOSTOP,
    /* 261 */ VGG_SCANCODE_AUDIOPLAY,
    /* 262 */ VGG_SCANCODE_AUDIOMUTE,
    /* 263 */ VGG_SCANCODE_MEDIASELECT,
    /* 264 */ VGG_SCANCODE_WWW,
    /* 265 */ VGG_SCANCODE_MAIL,
    /* 266 */ VGG_SCANCODE_CALCULATOR,
    /* 267 */ VGG_SCANCODE_COMPUTER,
    /* 268 */ VGG_SCANCODE_AC_SEARCH,
    /* 269 */ VGG_SCANCODE_AC_HOME,
    /* 270 */ VGG_SCANCODE_AC_BACK,
    /* 271 */ VGG_SCANCODE_AC_FORWARD,
    /* 272 */ VGG_SCANCODE_AC_STOP,
    /* 273 */ VGG_SCANCODE_AC_REFRESH,
    /* 274 */ VGG_SCANCODE_AC_BOOKMARKS,
    /* 275 */ VGG_SCANCODE_BRIGHTNESSDOWN,
    /* 276 */ VGG_SCANCODE_BRIGHTNESSUP,
    /* 277 */ VGG_SCANCODE_DISPLAYSWITCH,
    /* 278 */ VGG_SCANCODE_KBDILLUMTOGGLE,
    /* 279 */ VGG_SCANCODE_KBDILLUMDOWN,
    /* 280 */ VGG_SCANCODE_KBDILLUMUP,
    /* 281 */ VGG_SCANCODE_EJECT,
    /* 282 */ VGG_SCANCODE_SLEEP,
    /* 283 */ VGG_SCANCODE_APP1,
    /* 284 */ VGG_SCANCODE_APP2,
    /* 285 */ VGG_SCANCODE_AUDIOREWIND,
    /* 286 */ VGG_SCANCODE_AUDIOFASTFORWARD,
    /* 287 */ VGG_SCANCODE_SOFTLEFT,
    /* 288 */ VGG_SCANCODE_SOFTRIGHT,
    /* 289 */ VGG_SCANCODE_CALL,
    /* 290 */ VGG_SCANCODE_ENDCALL,
  };

  return static_cast<EVGGKeyCode>(s_keyMap[scancode]);
}
