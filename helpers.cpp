
#define _CRT_SECURE_NO_WARNINGS

#include <SDL3/SDL.h>
#include "engine_types.h"
#include "config.h"
#include "helpers.h"

unsigned char g_keyboard_buffer[256];


bool load_texture(cTexture& MyTexture, const wchar_t* szFileName, bool force256)
{
	char buff[255];

	std::wstring file = ASSETS_PATH;
	file += szFileName;
	std::wcstombs(buff, file.c_str(), 255);
	SDL_Surface* bm2 = SDL_LoadBMP(buff);

	if (bm2)
	{
		SDL_Surface* bm = SDL_ConvertSurface(bm2, SDL_PIXELFORMAT_ABGR32);
		SDL_DestroySurface(bm2);
		MyTexture.surface = bm2;
		MyTexture.pixels = (Colour<BYTE>*)bm->pixels;
		MyTexture.bmBytesPixel = bm->pitch / bm->w;
		MyTexture.bmBitsPixel = MyTexture.bmBytesPixel * 8;
		MyTexture.bmType = bm->format;
		MyTexture.filename = szFileName;
		MyTexture.bmWidth = bm->w;
		MyTexture.bmHeight = bm->h;
		MyTexture.bmWidthBytes = MyTexture.bmWidth * 4;
		MyTexture.faceB = 100;

		MyTexture.hasAlpha = false;

		return true;
	}

	return false;
}




// https://wiki.libsdl.org/SDL3/SDL_Scancode

void handle_key_event_down(SDL_Scancode code)
{
	if (code > 0 && code < 256) {
		if ((g_keyboard_buffer[code] & 0x01) == 0x00) g_keyboard_buffer[code] |= 0x01;
		if ((g_keyboard_buffer[code] & 0x04) == 0x00) g_keyboard_buffer[code] |= 0x02;
	}
}




void handle_key_event_up(SDL_Scancode code)
{
	if (code > 0 && code < 256)
	{
		if ((g_keyboard_buffer[code] & 0x04) == 0x04)
			g_keyboard_buffer[code] &= 0x00;
		else
			g_keyboard_buffer[code] &= 0xFE;
	}
}
