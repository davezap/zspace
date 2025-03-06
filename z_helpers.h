

#pragma once

#include <filesystem>
#include <regex>
#include <SDL3/SDL.h>
#include "z_types.h"
#include "config.h"
#include "z_helpers.h"



inline unsigned char g_keyboard_buffer[256];

void handle_key_event_down(SDL_Scancode code);
void handle_key_event_up(SDL_Scancode code);



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



inline bool load_texture(Texture& MyTexture, const std::string szFileName)
{
	std::string file = ASSETS_PATH;
	file += szFileName;
	SDL_Surface* bm2 = SDL_LoadBMP(file.c_str());

	if (bm2)
	{
		SDL_Surface* bm = SDL_ConvertSurface(bm2, SDL_PIXELFORMAT_ABGR32);
		SDL_DestroySurface(bm2);
		MyTexture.pixels_colour = (Colour<BYTE>*)bm->pixels;
		MyTexture.pixels_normal = NULL;

		MyTexture.bmBytesPixel = bm->pitch / bm->w;
		MyTexture.bmBitsPixel = MyTexture.bmBytesPixel * 8;
		MyTexture.bmType = bm->format;
		MyTexture.filename = szFileName;
		MyTexture.bmWidth = bm->w;
		MyTexture.bmHeight = bm->h;
		MyTexture.bmWidthBytes = MyTexture.bmWidth * 4;
		MyTexture.mat_diffuse = { 100,0,0 };

		MyTexture.hasAlpha = false;


		//  Try load surface normal texture.
		std::string file_normal = std::regex_replace(file, std::regex("\\_COL_"), "_NRM_");
		if(file_normal!=file && std::filesystem::exists(file_normal))
		{
			SDL_Surface* bm3 = SDL_LoadBMP(file_normal.c_str());
			if (bm3)
			{
				SDL_Surface* bm = SDL_ConvertSurface(bm3, SDL_PIXELFORMAT_ABGR32);
				SDL_DestroySurface(bm3);

				Colour<BYTE>* pm = (Colour<BYTE>*)bm->pixels;
				MyTexture.pixels_normal = static_cast<Vec3*>(SDL_malloc(MyTexture.bmWidth * MyTexture.bmHeight * 3 * sizeof(float)));
				for (z_size_t a = 0; a < MyTexture.bmWidth * MyTexture.bmHeight; a++)
				{
					MyTexture.pixels_normal[a] = pm[a].toNormal();
				}
			}
		}

		return true;
	}

	return false;
}




// https://wiki.libsdl.org/SDL3/SDL_Scancode

inline void handle_key_event_down(SDL_Scancode code)
{
	if (code > 0 && code < 256) {
		if ((g_keyboard_buffer[code] & 0x01) == 0x00) g_keyboard_buffer[code] |= 0x01;
		if ((g_keyboard_buffer[code] & 0x04) == 0x00) g_keyboard_buffer[code] |= 0x02;
	}
}




inline void handle_key_event_up(SDL_Scancode code)
{
	if (code > 0 && code < 256)
	{
		if ((g_keyboard_buffer[code] & 0x04) == 0x04)
			g_keyboard_buffer[code] &= 0x00;
		else
			g_keyboard_buffer[code] &= 0xFE;
	}
}

