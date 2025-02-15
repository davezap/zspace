#ifndef ZSPACE_ENGINE_H
#define ZSPACE_ENGINE_H  1


#include "LX.h"
#include "engine_types.h"

bool init_world();
void deinit_world();
void main_loop(Colour<BYTE>* src_pixels);
void render_text_overlay(SDL_Renderer* renderer);


#endif /* ZSPACE_ENGINE_H */