#ifndef ZRAY_HELPERS_H
#define ZRAY_HELPERS_H

#include "engine_types.h"

extern unsigned char g_keyboard_buffer[256];

bool load_texture(cTexture& MyTexture, const wchar_t* szFileName, bool force256);
inline Colour<BYTE> get_texture_pixel(cTexture& MyTexture, z_screen_t x, z_screen_t y);
void handle_key_event_down(SDL_Scancode code);
inline bool key_get_clear(SDL_Scancode code);
inline bool key_get(SDL_Scancode code);
void handle_key_event_up(SDL_Scancode code);



inline Colour<BYTE> get_texture_pixel(cTexture& MyTexture, z_screen_t x, z_screen_t y)
{
	x %= MyTexture.bmWidth;
	y %= MyTexture.bmHeight;
	return MyTexture.pixels[x + y * MyTexture.bmWidth];
}

inline bool key_get_clear(SDL_Scancode code)
{
	if (code > 0 && code < 256) {
		if ((g_keyboard_buffer[code] & 0x02) == 0x02)
		{
			g_keyboard_buffer[code] |= 0x04;
			g_keyboard_buffer[code] &= 0xFD;
			return true;
		}
	}
	return false;
}

inline bool key_get(SDL_Scancode code)
{
	if (code > 0 && code < 256 && g_keyboard_buffer[code] & 0x01) return true;
	return false;
}


#endif /* ZRAY_HELPERS_H */

