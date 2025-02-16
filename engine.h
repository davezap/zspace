#pragma once

#include "LX.h"
#include "z_types.h"

bool init_world();
void deinit_world();
void main_loop(Colour<BYTE>* src_pixels);
void render_text_overlay(SDL_Renderer* renderer);

