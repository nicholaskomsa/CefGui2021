

#include "SdlKeyCodesToWindows.h"

#include <sdl_keycode.h>
#include <SDL_keyboard.h>

#include <include/internal/cef_types.h>

int getKeyboardModifiers(const uint16_t mod)
{
	int result = EVENTFLAG_NONE;

	if (mod & KMOD_NUM)
	{
		result |= EVENTFLAG_NUM_LOCK_ON;
	}

	if (mod & KMOD_CAPS)
	{
		result |= EVENTFLAG_CAPS_LOCK_ON;
	}

	if (mod & KMOD_CTRL)
	{
		result |= EVENTFLAG_CONTROL_DOWN;
	}

	if (mod & KMOD_SHIFT)
	{
		result |= EVENTFLAG_SHIFT_DOWN;
	}

	if (mod & KMOD_ALT)
	{
		result |= EVENTFLAG_ALT_DOWN;
	}
	return result;
}

int getWindowsKeyCode(const SDL_Keysym& key)
{
	int result = 0;

	bool shift = key.mod & KMOD_SHIFT;
	bool caps = key.mod & KMOD_CAPS;
	bool alt_gr = key.mod & KMOD_RALT;
	bool uppercase = (caps && !shift) || (shift && !caps);


	switch (key.sym)
	{
	case SDLK_LEFT:
		result = 37;
		break;
	case SDLK_UP:
		result = 38;
		break;
	case SDLK_RIGHT:
		result = 39;
		break;
	case SDLK_DOWN:
		result = 40;
		break;

	case SDLK_F1:
		result = 112;
		break;
	case SDLK_F2:
		result = 113;
		break;
	case SDLK_F3:
		result = 114;
		break;
	case SDLK_F4:
		result = 115;
		break;
	case SDLK_F5:
		result = 116;
		break;
	case SDLK_F6:
		result = 117;
		break;
	case SDLK_F7:
		result = 118;
		break;
	case SDLK_F8:
		result = 119;
		break;
	case SDLK_F9:
		result = 120;
		break;
	case SDLK_F10:
		result = 121;
		break;
	case SDLK_F11:
		result = 122;
		break;
	case SDLK_F12:
		result = 123;
		break;
	case SDLK_F13:
		result = 124;
	case SDLK_F14:
		result = 125;
	case SDLK_F15:
		result = 126;
		break;
	case SDLK_F16:
		result = 127;
		break;

	case SDLK_0:
		result = shift ? ')' : '0';
		break;
	case SDLK_1:
		result = shift ? '!' : '1';
		break;
	case SDLK_2:
		result = shift ? '@' : '2';
		break;
	case SDLK_3:
		result = shift ? '#' : '3';
		break;
	case SDLK_4:
		result = shift ? '$' : '4'; // alt_gr ? 123 : uppercase ? 52 : 39; // { ? 4 : '
		break;
	case SDLK_5:
		result = shift ? '%' : '5';//  alt_gr ? 91 : uppercase ? 53 : 40; // [ ? 5 : ( (KO)
		break;
	case SDLK_6:
		result = shift ? '^' : '6'; // alt_gr ? 124 : uppercase ? 54 : 45; // | ? 6 : -
		break;
	case SDLK_7:
		result = shift ? '&' : '7'; // alt_gr ? 96 : uppercase ? 55 : 232; // ` ? 7 : è
		break;
	case SDLK_8:
		result = shift ? '*' : '8'; // alt_gr ? 92 : uppercase ? 56 : 95; // \ ? 8 : _
		break;
	case SDLK_9:
		result = shift ? '(' : '9'; // alt_gr ? 94 : uppercase ? 57 : 231; // ^ ? 9 : ç
		break;

	case SDLK_a:
		result = uppercase ? 65 : 97;
		break;
	case SDLK_b:
		result = uppercase ? 66 : 98;
		break;
	case SDLK_c:
		result = uppercase ? 67 : 99;
		break;
	case SDLK_d:
		result = uppercase ? 68 : 100;
		break;
	case SDLK_e:
		result = uppercase ? 69 : 101;
		break;
	case SDLK_f:
		result = uppercase ? 70 : 102;
		break;
	case SDLK_g:
		result = uppercase ? 71 : 103;
		break;
	case SDLK_h:
		result = uppercase ? 72 : 104;
		break;
	case SDLK_i:
		result = uppercase ? 73 : 105;
		break;
	case SDLK_j:
		result = uppercase ? 74 : 106;
		break;
	case SDLK_k:
		result = uppercase ? 75 : 107;
		break;
	case SDLK_l:
		result = uppercase ? 76 : 108;
		break;
	case SDLK_m:
		result = uppercase ? 77 : 109;
		break;
	case SDLK_n:
		result = uppercase ? 78 : 110;
		break;
	case SDLK_o:
		result = uppercase ? 79 : 111;
		break;
	case SDLK_p:
		result = uppercase ? 80 : 112;
		break;
	case SDLK_q:
		result = uppercase ? 81 : 113;
		break;
	case SDLK_r:
		result = uppercase ? 82 : 114;
		break;
	case SDLK_s:
		result = uppercase ? 83 : 115;
		break;
	case SDLK_t:
		result = uppercase ? 84 : 116;
		break;
	case SDLK_u:
		result = uppercase ? 85 : 117;
		break;
	case SDLK_v:
		result = uppercase ? 86 : 118;
		break;
	case SDLK_w:
		result = uppercase ? 87 : 119;
		break;
	case SDLK_x:
		result = uppercase ? 88 : 120;
		break;
	case SDLK_y:
		result = uppercase ? 89 : 121;
		break;
	case SDLK_z:
		result = uppercase ? 90 : 122;
		break;

	case SDLK_HOME:
		result = 36;
		break;
	case SDLK_END:
		result = 35;
		break;
	case SDLK_INSERT:
		result = 45;
		break;
	case SDLK_DELETE:
		result = 46;
		break;
	case SDLK_PAGEDOWN:
		result = 34;
		break;
	case SDLK_PAGEUP:
		result = 33;
		break;

	case SDLK_RETURN:
		result = 13;
		break;
	case SDLK_ESCAPE:
		result = 27;
		break;
	case SDLK_BACKSPACE:
		result = 8;
		break;
	case SDLK_TAB:
		result = 9;
		break;
	case SDLK_SPACE:
		result = ' ';
		break;

	case SDLK_LEFTBRACKET:
		result = shift ? '{' : '[';
		break;
	case SDLK_RIGHTBRACKET:
		result = shift ? '}' : ']';
		break;
	case SDLK_BACKQUOTE:
		result = shift ? '~' : '`';
		break;
	case SDLK_QUOTE:
		result = shift ? '"' : '\'';
		break;
	case SDLK_COMMA:
		result = shift ? '<' : ',';
		break;
	case SDLK_MINUS:
		result = shift ? '_' : '-';
		break;
	case SDLK_UNDERSCORE:
		result = '_';
		break;
	case SDLK_EQUALS:
		result = shift ? '+' : '=';
		break;
	case SDLK_PERIOD:
		result = shift ? '>' : '.';
		break;
	case SDLK_SLASH:
		result = shift ? '?' : '/';
		break;
	case SDLK_BACKSLASH:
		result = shift ? '|' : '\\';
		break;
	case SDLK_SEMICOLON:
		result = shift ? ':' : ';';
		break;

	}

	return result;
}