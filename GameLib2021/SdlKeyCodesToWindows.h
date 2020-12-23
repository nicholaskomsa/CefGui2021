#pragma once

struct SDL_Keysym;

#include <SDL_config_windows.h>


int getKeyboardModifiers(const uint16_t mod);
int getWindowsKeyCode(const SDL_Keysym&  key);