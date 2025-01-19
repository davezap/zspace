#ifndef ZSPACE_ENGINE_H
#define ZSPACE_ENGINE_H  1


#include "LX.h"
#include "engine_types.h"

void DeInit();
void Init();
char* Game_Main(unsigned char* lpSurface);
void render_debug(SDL_Renderer* renderer);

void handle_key_event_down(SDL_Scancode key_code);
void handle_key_event_up(SDL_Scancode key_code);



#endif /* ZSPACE_ENGINE_H */