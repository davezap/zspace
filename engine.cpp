/*
	 zSpace code.
	 Daves 3D Engine. 

Copyright (c) 2002-2006, David Chamberlain - DaveZ@204am.com

*/
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define INITGUID
#include <iostream>
#include <sstream>
#include <format>
#include <SDL3/SDL.h>

#include "z_types.h"
#include "z_helpers.h"
#include "config.h"
#include "engine.h"

void process_input(void);
void update_variables(void);
void go_boom(Vec3 pos, bool cast_shrap, bool force);
void go_shrapnel(Vec3 pos, unsigned char sh_type, unsigned int scnt);
long get_free_sprite(bool force);
void render_background_sprites(Sprite objs[], unsigned int& objCnt);
void render_foreground_sprites(void);
void render_tf();
void rotate_world(void);
void mesh_lights();


ZSpace_World g_world;	// Contains 3D objects.
Colour<BYTE>* g_video_buffer = NULL;	// Pointers for our screen keyboard_buffer, A B G R bytes.
Colour<BYTE>* g_video_bufferB = NULL;	// tmp keyboard_buffer for the same purpose.
Uint64 g_timer_render;		// Tracks the number of ms to render one frame
Uint64 g_fps_Time = 0;		// Tracks time for FPS counter.

z_size_t g_rendered_sprites = 0; // Statistics...
z_size_t g_rendered_objects = 0;
z_size_t g_total_faces = 0;
z_size_t g_rendered_faces = 0;

// Variables relating to the Camera object (our fighter)
Vec3 g_camera_angle = { 0, 0, 0 };
Vec3 g_camera_position = { 0, 0,0 };
float g_cam_zoom_y = -20;
z_size_t g_cam_object_idx = 0;
const float g_view_distance = 300;		// Sort of like field of view.

// Indexes in to sprite textures.
z_size_t g_shoot_sprite_idx = 0;
z_size_t g_shrapnel_sprite_idx = 0;
z_size_t g_boom_sprite_idx = 0;
z_size_t g_shot_sprite_len = 0;	// and animation lengths if applicable.
z_size_t g_shrapnel_sprite_len = 0;
z_size_t g_boom_sprite_len = 0;




float g_scale = 5;

std::vector<Texture> g_textures;
const unsigned int mines_cnt = 100;

const unsigned int SCREEN_WIDTHh = SCREEN_WIDTH / 2;
const unsigned int SCREEN_HEIGHTh = SCREEN_HEIGHT / 2;




Sprite* sprites_back = 0;
z_size_t sprites_back_cnt = 0;
Sprite* sprites_front = 0;
z_size_t sprites_front_cnt = 0;
Sprite* sprite_tmp = 0;

long SelTxr = 0;
long chk_TextureShow = 0;

float ZBuffer[SCREEN_WIDTH * SCREEN_HEIGHT] = {};
float* pZBuffer = 0;
long ZBuffer_Offset = 0;
long g_zbuffer_page = 0;

ZSpace_Object* camobj = 0;
ZSpace_Object* curobj = 0;



Clipped_Poly pclip;


double globalprint = 0.0f;
double debug1 = 0.0f;
double debug2 = 0.0f;
double debug3 = 0.0f;

// USING DECLARATION //////////////////////////////////////////////////////////////////////////////






void dbg(const wchar_t* szFormat, ...)
{

	wchar_t  szBuff[1024];
	va_list arg;
	va_start(arg, szFormat);
	_vsnwprintf_s(szBuff, sizeof(szBuff), szFormat, arg);
	va_end(arg);
	OutputDebugString(szBuff);


}



// GLOBALS ////////////////////////////////////////////////////////////////////////////////////////




// FUNCTIONS //////////////////////////////////////////////////////////////////////////////////////


// draws entire screen to test worst case time..
void screen_test()
{
	unsigned char* p;
	//video_buffer = video_bufferB;
	for (int x = 0; x < SCREEN_WIDTH; x++) {
		for (int y = 0; y < SCREEN_HEIGHT; y++) {
			g_video_buffer = &g_video_bufferB[x + y * SCREEN_WIDTH];
			g_video_buffer->a = 1;
			g_video_buffer->r = x & 100;
			g_video_buffer->g = x & 100;
			g_video_buffer->b = y % 135;
		}
	}
	//frameOffset += 0.005f;
}


// Game_Main () ///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// Main game loop /////////////////////////////////////////////////////////////////////////////////

void main_loop(Colour<BYTE>* src_pixels)
{


	g_timer_render = SDL_GetTicks();

	// Store pointer to video-memory
	g_video_buffer = g_video_bufferB = src_pixels;

	/////////////////////////////////////////////
	////// Begin Render /////////////////////////
	if (0) {
		screen_test();
	}
	else {

		rotate_world();

		// Clear ZBuffer if needed 

		if(g_zbuffer_page==100)
		{
			g_zbuffer_page = 1;
			memset(ZBuffer,1,sizeof(ZBuffer));
		} else g_zbuffer_page++;

		g_rendered_sprites = 0;
		g_rendered_objects = 0;
		g_total_faces = 0;
		g_rendered_faces = 0;

		render_background_sprites(sprites_back, sprites_back_cnt);
		render_tf();
		render_foreground_sprites();
	
	
  
		/////////////////////////////////////////////
		////// End Render /////////////////////////

		// Process keyboard input
		process_input();

	
		// Maintain variables within limits
		update_variables ();
	}

	g_timer_render = SDL_GetTicks() - g_timer_render;
	// Return success
	//return ( NULL );

}

void render_text_overlay(SDL_Renderer* renderer)
{
	Uint64 tmr = SDL_GetTicks() - g_fps_Time;
	g_fps_Time = SDL_GetTicks();
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 200);

	std::string debug_buffer = std::format("Rendered Sprites {}/{} - Objects {}/{} - Faces {}/{}", g_rendered_sprites, sprites_back_cnt + sprites_front_cnt, g_rendered_objects, 1, g_rendered_faces, g_total_faces); //g_world.obj.size()
	SDL_RenderDebugText(renderer, 0, 5, debug_buffer.c_str());

	debug_buffer = std::format("{} fps, render {} ms", 1000 / tmr, g_timer_render);
	//dbg(L"te=%d\n\r", g_timer_render);
	SDL_RenderDebugText(renderer, 0, 20, debug_buffer.c_str());
	
}


void process_input ( void )
{
	#define KEYDOWN(name, key) (name[key] & 0x80) 

	if (g_keyboard_buffer[SDL_SCANCODE_Q]) {
		g_fps_Time = SDL_GetTicks();
	}

	if (key_get(SDL_SCANCODE_W))	g_cam_zoom_y=0;
	if (key_get(SDL_SCANCODE_Z))	g_cam_zoom_y+=10;
	if (key_get(SDL_SCANCODE_X))	g_cam_zoom_y-=10;
	if (key_get(SDL_SCANCODE_A))	g_cam_zoom_y+=1;
	if (key_get(SDL_SCANCODE_S))	g_cam_zoom_y-=1;
	
	if (key_get(SDL_SCANCODE_B))	g_camera_angle.x=-90;
	if (key_get(SDL_SCANCODE_N))	g_camera_angle.x++;
	if (key_get(SDL_SCANCODE_M))	g_camera_angle.x--;

	if (key_get(SDL_SCANCODE_G))	g_camera_angle.y++;
	if (key_get(SDL_SCANCODE_H))	g_camera_angle.y--;
	
	if (key_get(SDL_SCANCODE_J))	g_camera_angle.z++;
	if (key_get(SDL_SCANCODE_K))	g_camera_angle.z--;
	
	 //g_world.obj[g_cam_object_idx]

	if (key_get(SDL_SCANCODE_C))	go_shrapnel(g_world.obj[g_cam_object_idx]->position,1,100);
	if (key_get(SDL_SCANCODE_RIGHT))
	{
		g_world.obj[g_cam_object_idx]->turnpitch = 0.5f;
	}
	if (key_get(SDL_SCANCODE_LEFT))
	{
		g_world.obj[g_cam_object_idx]->turnpitch = -0.5f;
	}
	if(g_world.obj[g_cam_object_idx]->angle.y > 359) g_world.obj[g_cam_object_idx]->angle.y = g_world.obj[g_cam_object_idx]->angle.y - 360;
	if(g_world.obj[g_cam_object_idx]->angle.y < 0) g_world.obj[g_cam_object_idx]->angle.y = 360 + g_world.obj[g_cam_object_idx]->angle.y;
	//if(WLD.obj[0].an_Z>359) WLD.obj[0].an_Z = WLD.obj[0].an_Z - 360;
	//if(WLD.obj[0].an_Z<0) WLD.obj[0].an_Z = 360 + WLD.obj[0].an_Z;

	
	if (key_get(SDL_SCANCODE_SPACE))
	{
		g_world.obj[g_cam_object_idx]->shoot = true;
	}

	if (key_get(SDL_SCANCODE_UP))
	{
		g_world.obj[g_cam_object_idx]->thrust = 1;
	}

	if (key_get(SDL_SCANCODE_DOWN))
	{
		g_world.obj[g_cam_object_idx]->thrust = -1;
	}

	/*
	if ( keyboard_buffer[DIK_RALT))	input_scale *= 3;
	if ( keyboard_buffer[DIK_EQUALS))	cube.zPos   += ( 3 * input_scale );
	if ( keyboard_buffer[DIK_MINUS))	cube.zPos   -= ( 3 * input_scale );
	if ( keyboard_buffer[DIK_LBRACKET))	speed -= input_scale;
	if ( keyboard_buffer[DIK_RBRACKET))	speed += input_scale;
*/

}


void go_boom(Vec3 pos, bool cast_shrap,bool force)
{
	
	Vec3 tvec;

	for(z_size_t tmp=0;tmp < sprites_front_cnt;tmp++)
	{

		if(sprites_front[tmp].visable)
		{
			tvec = sprites_front[tmp].position - pos;
			if(tvec.length() < 5)
			{
				return;
			}
		}
	}

	long smObj = get_free_sprite(force);
	if(smObj==-1) return;
	sprites_front[smObj].position.x = pos.x;
	sprites_front[smObj].position.y = -(float)(rand()%30);
	sprites_front[smObj].position.z = pos.z;
	sprites_front[smObj].v.x = 0;
	sprites_front[smObj].v.z = 0;
	sprites_front[smObj].v.y = 0;
	sprites_front[smObj].liveTime = g_boom_sprite_len;
	sprites_front[smObj].visable = true;
	sprites_front[smObj].animation_start = g_boom_sprite_idx;
	sprites_front[smObj].animation_len = g_boom_sprite_len;
	sprites_front[smObj].boTexture = g_boom_sprite_idx;
	sprites_front[smObj].alpha = 0.8;
	sprites_front[smObj].scale = true;

	if(cast_shrap) go_shrapnel(pos,0,2);
}

long get_free_sprite (bool force)
{
	int smLiveTime = 10000;
	z_size_t smObj = -1;
	for(z_size_t tmp=0;tmp < sprites_front_cnt;tmp++)
	{
		if(sprites_front[tmp].visable == false)
		{
			smObj = tmp;
			break;
		}
		if(force && sprites_front[tmp].liveTime<smLiveTime && sprites_front[tmp].animation_start != g_shoot_sprite_idx)
		{
			smObj = tmp;
			smLiveTime = sprites_front[tmp].liveTime;
		}
	}

	return smObj;

}
void go_shrapnel(Vec3 pos,unsigned char sh_type, unsigned int scnt)
{
	z_size_t smObj;
	for(unsigned int t=1;t<scnt;t++)
	{
		smObj = get_free_sprite(false);
		if(smObj==-1) return;
		//debug3 = sprites_front[tmp].visable;
		sprites_front[smObj].visable = true;
		sprites_front[smObj].position.x = pos.x;
		sprites_front[smObj].position.z = pos.z;
		sprites_front[smObj].position.y =  0;
		float ay = static_cast<float>(rand()%360);
		sprites_front[smObj].v.z = (MyCos(ay)/10 * 50);
		sprites_front[smObj].v.x = (MySin(ay)/10 * 50);
		sprites_front[smObj].liveTime = 190;
		if(sh_type==0)
		{
			sprites_front[smObj].v.y = float(rand() % 100 - 50) / 50;
			sprites_front[smObj].v.z = float(rand() % 100 - 50) / 50;
			sprites_front[smObj].v.x = 0;// /= 2;

			sprites_front[smObj].animation_start = g_shrapnel_sprite_idx;
			sprites_front[smObj].animation_len = g_shrapnel_sprite_len;
			sprites_front[smObj].boTexture = g_shrapnel_sprite_idx;
			sprites_front[smObj].alpha = 1;
		} else {
			sprites_front[smObj].v.y = 0;
			sprites_front[smObj].animation_start = g_shoot_sprite_idx;
			sprites_front[smObj].animation_len = g_shot_sprite_len;
			sprites_front[smObj].boTexture = g_shoot_sprite_idx;
			sprites_front[smObj].alpha = 1;
		}	
	}

}




void update_variables ( void )
{
	Vec3 tvector = { 0,0,0 };
	ZSpace_Object *curobj;


	// Ship is shooting so make a rainbow pompom fly out the front end..

	if(camobj->shoot)
	{
		camobj->shoot = false;
		float ay = camobj->angle.y;
		z_size_t smObj = get_free_sprite(true);
		sprite_tmp = &sprites_front[smObj];
		if(smObj==-1) return;
		//debug3 = sprites_front[tmp].visable;
		sprite_tmp->visable = true;
		sprite_tmp->position =  camobj->position;
		sprite_tmp->v.z = (MyCos(ay)/10 * 50)+camobj->v.z;
		sprite_tmp->v.x = (MySin(ay)/10 * 50)-camobj->v.x;
		sprite_tmp->v.y = 0;
		sprite_tmp->liveTime = 190;
		sprite_tmp->animation_start = g_shoot_sprite_idx;
		sprite_tmp->animation_len = g_shot_sprite_len;
		sprite_tmp->boTexture = g_shoot_sprite_idx;
		sprite_tmp->alpha = 1;
	}

	
	// Handle collisions between object<>CamOb, object<>object and object<>sprites.
	// Also to physics updates.
	// CamOb is the index of our ship, that the camera is atached to.

	for (z_size_t ob = 0; ob < g_world.obj.size(); ob++)
	{
		curobj = g_world.obj[ob].get();

		if (!curobj->scale) continue;	// Destroyed.

		// This just causes object to magically stay contained in a given area.
		// just like the good old days.
		BOOL moved = false;
		if (curobj->position.x < -500) {curobj->position.x = 500; moved = true;}
		if (curobj->position.x > 500) {curobj->position.x = -500; moved = true;}
		if (curobj->position.y < -500) { curobj->position.y = 500; moved = true; }
		if (curobj->position.y > 500) { curobj->position.y = -500; moved = true; }
		if (curobj->position.z < -500) { curobj->position.z = 500; moved = true; }
		if (curobj->position.z > 500) { curobj->position.z = -500; moved = true; }

		if (moved)
		{
			curobj->hidden = false;
			for (z_size_t b = 0; b < curobj->polycount; b++)
			{
				curobj->polygon[b].visable = 0;
			}
		}

		if(ob!=g_cam_object_idx)
		{	
			// Our method of astroid destruction is to gradually shrink it as it takes damage.
			// When it's small enough go boom.
			if (curobj->scale < 1 && curobj->scale != 0) {
				curobj->scale = 0;
				//curobj->y = -1000;
				//if(!curobj->hidden)
				go_boom(curobj->position, true, true);
				go_shrapnel(curobj->position, 1, 2);
				go_shrapnel(curobj->position, 0, 2);
				continue;
			}

			// Each object has a volocity (v.x, v.y, v.z), and angular velocity (vax, vay, vaz)
			curobj->angle += curobj->va;
			curobj->angle.roll360();
			curobj->position += curobj->v;
		}
		
		// Test curobj against all other objects/sprites for collisions this includes our ship.
		for(z_size_t obb=ob+1;obb<g_world.obj.size();obb++)
		{
			if (g_world.obj[obb]->scale!=0) // && ob != CamOb
			{
				ZSpace_Object* curobj2 = g_world.obj[obb].get();
				tvector = curobj2->position - curobj->position;
				float distance = tvector.length();
				if(distance < ((curobj->bounds + curobj2->bounds)))
				{
					float ux = tvector.x / distance;
					float uz = tvector.z / distance;
					float rvx = curobj2->v.x - curobj->v.x;
					float rvz = curobj2->v.z - curobj->v.z;
					float vNormal = rvx * ux + rvz * uz;
					if (vNormal > 0) continue;
					float impulse = (1 * vNormal) / (curobj->mass + curobj2->mass);
					curobj->v.x += impulse * curobj2->mass * ux;
					curobj->v.z += impulse * curobj2->mass * uz;
					curobj2->v.x -= impulse * curobj->mass * ux;
					curobj2->v.z -= impulse * curobj->mass * uz;
					
					//Boom(curobj->x,curobj->z,false,false);
				}
			}
		}

		// does particle collision..
		if(ob!=g_cam_object_idx)
		{
			for(z_size_t a = 0;a < sprites_front_cnt;a++)
			{
				sprite_tmp = &sprites_front[a];
				if(sprite_tmp->visable == true && sprite_tmp->animation_start == g_shoot_sprite_idx)
				{
					tvector = { curobj->position.x - sprite_tmp->position.x, 0, curobj->position.z - sprite_tmp->position.z };
					if(tvector.length() < curobj->bounds)
					{
						curobj->scale -= 0.001f;
						sprites_front[a].visable = false;
						if(sprite_tmp->animation_start == g_shoot_sprite_idx)
						{
							tvector.x = sprite_tmp->position.x;
							tvector.z = sprite_tmp->position.z;
							curobj->scale -=0.05f;
							//Shrap(tx, tz, 0, 5);
							//if(!curobj->hidden)
							go_boom(tvector, true, true);
						}
							
							
					}

				}
			}
				
		}

	}

	if(camobj->thrust!=0)
	{
		float ay = round(camobj->angle.y);
		camobj->v.z+=MyCos(ay)/10 * camobj->thrust;
		camobj->v.x-=MySin(ay)/10 * camobj->thrust;
	}

	if(camobj->v.z > 10) camobj->v.z = 10;
	if(camobj->v.x > 10) camobj->v.x = 10;
	if(camobj->v.z < -10) camobj->v.z = -10;
	if(camobj->v.x < -10) camobj->v.x = -10;

	camobj->position.z += camobj->v.z;
	camobj->position.x -= camobj->v.x;

	if(camobj->v.z < 0) camobj->v.z+=static_cast<float>(0.01);
	if(camobj->v.z > 0) camobj->v.z-=static_cast<float>(0.01);
	if(camobj->v.x < 0) camobj->v.x+=static_cast<float>(0.01);
	if(camobj->v.x > 0) camobj->v.x-=static_cast<float>(0.01);

	// Highlight 
	g_textures[1].mat_diffuse.r = 255 * abs((int)camobj->thrust);
	if (g_textures[1].mat_diffuse.r > 255) g_textures[1].mat_diffuse.r = 255;
	g_textures[5].mat_diffuse = g_textures[1].mat_diffuse;
	
	camobj->thrust = 0;

	if(camobj->turnpitch)
	{
		camobj->van.y += camobj->turnpitch;
		
		if(camobj->van.y < -5) camobj->van.y = -5;
		if(camobj->van.y > 5) camobj->van.y = 5;
		//dbg(L"%f %f\n", camobj->turnpitch, camobj->van.y);
		camobj->turnpitch = 0;
		
	} else {
		/*
		if (camobj->angle.z > 315)
			camobj->angle.z += (camobj->angle.z - 360) / 1.05f;
		else
			if(camobj->angle.z!=0) camobj->angle.z/=1.05f;
		*/
	}

	if (camobj->van.y) camobj->van.y /= 1.1;

	camobj->angle.z = camobj->van.y * 5;
	camobj->angle.y += camobj->van.y;
	camobj->angle.roll360();
	

	g_camera_position.y = camobj->position.y -(190 + g_cam_zoom_y);// -((sqrt(camobj->v.x * camobj->v.x + camobj->v.z * camobj->v.z) * 10) + 200 + CamZoomY);
	g_camera_position.x = camobj->position.x;
	g_camera_position.z = camobj->position.z;

	
	g_camera_position.z = camobj->position.z - (-g_camera_position.y * MyCos(g_camera_angle.x));
	g_camera_position.y = camobj->position.y - (g_camera_position.y * MySin(g_camera_angle.x));

	
	for(z_size_t a = 0;a < sprites_front_cnt;a++)
	{
		sprite_tmp = &sprites_front[a];
		if(sprite_tmp->visable == false) continue;
		
		
		if(sprite_tmp->animation_start != g_boom_sprite_idx) sprite_tmp->alpha-=static_cast<float>(0.005);
		debug3++;


		//sprite_tmp->v.x = (curobj->x-sprite_tmp->x)/100;
		//sprite_tmp->v.z = (curobj->z-sprite_tmp->z)/100;


		if(sprite_tmp->animation_len > 0)
		{
			//debug3 = 
			if(sprite_tmp->liveTime > sprite_tmp->animation_len)
			{
				sprite_tmp->boTexture = sprite_tmp->animation_start + rand()%sprite_tmp->animation_len;
			} else {
				sprite_tmp->boTexture = sprite_tmp->animation_start + (sprite_tmp->animation_len - sprite_tmp->liveTime);
			}
		}
		sprite_tmp->position += sprite_tmp->v;

		sprite_tmp->liveTime--;
		if (sprite_tmp->liveTime == 0)
		{
			sprite_tmp->visable = false;
			sprite_tmp->liveTime = 0;
			continue;
		}
	}
	
}



inline void render_background_sprites(Sprite objs[], unsigned int &objCnt)
{

	//return;
	Colour<BYTE> *MyTextureP;
	float qqqc = 0;
	int h_run_distance;
	int runstart;
	int runtostart;

	int v_run_distance;
	
	int videoXOff;
	int videoXOffb;
	int videoYOff;
	//unsigned int sprite_width;
	//unsigned int sprite_height;

	for(z_size_t tmp=0;tmp<objCnt;tmp++)
	{
		sprite_tmp = &objs[tmp];
		
		if(!sprite_tmp->visable) continue;

		if(sprite_tmp->p)
		{
			sprite_tmp->l.z = 1/sprite_tmp->l.z;
			sprite_tmp->l.x = ((sprite_tmp->l.x) * sprite_tmp->l.z * g_view_distance) + SCREEN_WIDTHh;
			sprite_tmp->l.y = ((sprite_tmp->l.y) * sprite_tmp->l.z * g_view_distance) + SCREEN_HEIGHTh;
		} else {
			sprite_tmp->l.x = sprite_tmp->position.x + SCREEN_WIDTHh;
			sprite_tmp->l.y = sprite_tmp->position.y + SCREEN_HEIGHTh;
		}
		if(sprite_tmp->l.z < 0) continue;
		if(sprite_tmp->l.x < 0 || sprite_tmp->l.x>SCREEN_WIDTH || sprite_tmp->l.y < 0 || sprite_tmp->l.y>SCREEN_HEIGHT) continue;
		//float tsc;
		//if(sprite_tmp->l.y > SCREEN_HEIGHT && sprite_tmp->l.y < 0 && sprite_tmp->l.x > SCREEN_WIDTH && sprite_tmp->l.x < 0) continue;


		//continue;
		// Get pointer to this background objects texture
		Texture *ThisTexture = &g_textures[sprite_tmp->boTexture];
		MyTextureP = ThisTexture->pixels_colour;

		// Get video offset of this sprite
		videoXOff = static_cast<int>(sprite_tmp->l.x - ThisTexture->bmWidth / 2);
		videoYOff = static_cast<int>(sprite_tmp->l.y - ThisTexture->bmHeight / 2);

		h_run_distance = ThisTexture->bmWidth + videoXOff; //  + CamX


		if(videoXOff < 0)					// Clip to left of screen
		{
			if(h_run_distance<1) continue;		// off screen left, next...
			runstart = -videoXOff;
			videoXOff = 0;
			runtostart = 0;

		} else if(h_run_distance > SCREEN_WIDTH ) {	// Clip right

			if(videoXOff > SCREEN_WIDTH) continue;
			runstart = 0;
			h_run_distance = SCREEN_WIDTH - videoXOff;
			runtostart = (ThisTexture->bmWidth - h_run_distance);

		} else {

			h_run_distance = ThisTexture->bmWidth;
			runstart = 0;
			runtostart = 0;
		}


		if(videoYOff < 0)
		{

			if(videoYOff + ThisTexture->bmHeight < 1) continue;
			MyTextureP+= (-videoYOff) * ThisTexture->bmWidth;
			v_run_distance = videoYOff + ThisTexture->bmHeight;
			videoYOff = 0;

		} else if(ThisTexture->bmHeight + videoYOff > SCREEN_HEIGHT)
		{

			if(videoYOff > SCREEN_HEIGHT) continue;
			v_run_distance = SCREEN_HEIGHT - videoYOff;

		} else {

			v_run_distance = ThisTexture->bmHeight;	

		}

		videoXOffb = videoXOff;
		videoXOff++;

		if(sprite_tmp->zbuffer)
		{
			g_video_buffer = &g_video_bufferB[(0 + videoYOff) * SCREEN_WIDTH + videoXOff];
			pZBuffer = &ZBuffer[(0 + videoYOff) * SCREEN_WIDTH + videoXOffb];	
			for(int y = 0; y < v_run_distance; y++)
			{
				//video_buffer = &video_bufferB[(y + videoYOff) * video_buffer_row_size + videoXOff];
				//pZBuffer = &ZBuffer[(y + videoYOff) * SCREEN_WIDTH + videoXOffb];	
				MyTextureP+=runstart;

				for(int x = 0; x < h_run_distance; x++)
				{ 
					if(MyTextureP->r + MyTextureP->g + MyTextureP->b)
					{
						if((qqqc = sprite_tmp->l.z+g_zbuffer_page) > pZBuffer[0])
						{
							pZBuffer[0] = qqqc;	
							g_video_buffer = MyTextureP;
						}
					}

					g_video_buffer++;
					MyTextureP++;
					pZBuffer++;
				}

				MyTextureP+=runtostart;

			}
		} else {
			for(int y = 0; y<v_run_distance ;y++)
			{
				g_video_buffer = g_video_bufferB + (y + videoYOff) * SCREEN_WIDTH + videoXOff;
				MyTextureP+=runstart;

				for(int x = 0; x<h_run_distance;x++)
				{
					*g_video_buffer = *MyTextureP;

					g_video_buffer++;
					MyTextureP++;
				}

				MyTextureP+=runtostart;
			}
		}

		g_rendered_sprites += 1;

	}


}



inline void render_foreground_sprites(void)
{
	//debug1 = SDL_GetTicks();
	//debug1 = SDL_GetTicks();


	Sprite *obj;
	Colour<BYTE> *MyTexturePP;
	Colour<BYTE> *MyTextureP;

	int offset_x = 0;
	int offset_y = 0;
	int sprite_w = 0;
	int sprite_h = 0;
	int sprite_sw = 0;
	int sprite_sh = 0;
	int sprite_st = 0;
	int sprite_sl = 0;
	int video_rx = 0;
	float scaleStep = 0;
	float scaleCnt_st = 0; 
	float objalpha = 0;
	float objalphaInv = 0;
	float scaleCnt = 0;
	float qqqc = 0;
	int video_rxz = 0;
	float textureVpos = 0;
	float textureVstep = 0;
	float alpha = 0;
	float alphaInv = 0;
	int y = 0;
	int x = 0;
	Texture *ThisTexture;

	for(z_size_t tmp=0;tmp<sprites_front_cnt;tmp++)
	{
	
		obj = &sprites_front[tmp];

		if(!obj->visable) continue;
		obj->l.z = 1/obj->l.z;
		obj->l.x = ((obj->l.x) * obj->l.z * g_view_distance) + SCREEN_WIDTHh;
		obj->l.y = ((obj->l.y) * obj->l.z * g_view_distance) + SCREEN_HEIGHTh;
		if(obj->l.x < 0 || obj->l.x>SCREEN_WIDTH || obj->l.y < 0 || obj->l.y>SCREEN_HEIGHT) continue;
		if(obj->l.z<0)continue;

		// Get pointer to this background objects texture
		ThisTexture = &g_textures[obj->boTexture];
		//\scale = 2;
		// Get video offset of this sprite
		offset_x = static_cast<int>(obj->l.x - (g_scale * ThisTexture->bmWidth / 2));
		offset_y = static_cast<int>(obj->l.y - (g_scale * ThisTexture->bmHeight / 2));
		g_scale = obj->l.z * 200 * obj->oscale;
		//g_scale = 1;
		//debug1 = obj->l.z;

		sprite_w = ThisTexture->bmWidth;
		sprite_h = ThisTexture->bmHeight;
		sprite_sw = static_cast<int>(sprite_w * g_scale);
		sprite_sh = static_cast<int>(sprite_h * g_scale);
		sprite_st = 0;
		sprite_sl = 0;


		// Video Set up
		//int video_sx = offset_x * 3; 
		video_rx = SCREEN_WIDTH - sprite_sw;
		// Sprite Set up
		scaleStep= 1 / g_scale;
		scaleCnt_st = scaleStep; 

		// Clip Extense.
		if(offset_y<0)
		{
			if(offset_y + sprite_sh < 1 ) continue;
			sprite_st = -offset_y;
			offset_y = 0;
		}
		if(offset_y + sprite_sh > SCREEN_HEIGHT)
		{
			if(offset_y >= SCREEN_HEIGHT) continue;
			sprite_sh = int(SCREEN_HEIGHT - offset_y);
		}

		if(offset_x<0)
		{
			if(offset_x + sprite_sw < 1) continue;
			sprite_sl = -offset_x;
			scaleCnt_st = sprite_sl * scaleStep;
			video_rx = int(SCREEN_WIDTH - sprite_sw - offset_x);
			offset_x = 0;
		} 
		if (offset_x + sprite_sw > SCREEN_WIDTH)
		{
			if(offset_x >= SCREEN_WIDTH) continue;
			sprite_sw = SCREEN_WIDTH - offset_x;
			video_rx = int(SCREEN_WIDTH - sprite_sw);
		}

		//float alpha = obj->alpha;
		objalpha = obj->alpha;
		objalphaInv = 1 - obj->alpha;

		MyTextureP = ThisTexture->pixels_colour;
		MyTexturePP = MyTextureP;

		scaleCnt = 0;
		qqqc = 0;
		video_rxz = video_rx;
	

		g_video_buffer = &g_video_bufferB[offset_y * SCREEN_WIDTH + offset_x];
		pZBuffer = &ZBuffer[offset_y * SCREEN_WIDTH + offset_x];
	
		textureVpos = sprite_st / g_scale;
		textureVstep = (sprite_sh / g_scale - textureVpos) / (sprite_sh - sprite_st);;
		sprite_w = sprite_w;

		for(y = sprite_st; y < sprite_sh; y++)
		{
			scaleCnt = scaleCnt_st;
			MyTexturePP = &ThisTexture->pixels_colour[int(textureVpos) * sprite_w];

			if(objalpha!=1)
			{

				for(x = sprite_sl; x < sprite_sw;x++)
				{
					if((qqqc = obj->l.z+g_zbuffer_page) > pZBuffer[0])
					{
						MyTextureP = &MyTexturePP[int(scaleCnt)];
						//Output(L"x=%i y=%i p=%p\r\n", x, y, MyTextureP);
						//Output(L"%i %i %i\r\n", MyTextureP[0] , MyTextureP[1] , MyTextureP[2]);
						if(MyTextureP->r + MyTextureP->g + MyTextureP->b)
						{

							pZBuffer[0] = qqqc;
							g_video_buffer->r = MyTextureP->r * objalpha + g_video_buffer->r * objalphaInv;
							g_video_buffer->g = MyTextureP->g * objalpha + g_video_buffer->g * objalphaInv;
							g_video_buffer->b = MyTextureP->b * objalpha + g_video_buffer->b * objalphaInv;
						}
					}

					g_video_buffer++;
					scaleCnt+=scaleStep;
					pZBuffer++;
				}
			} else {
				if(ThisTexture->hasAlpha)
				{

					for(x = sprite_sl; x < sprite_sw;x++)
					{
						if((qqqc = obj->l.z+g_zbuffer_page) > pZBuffer[0])
						//{
							//pZBuffer[0] = qqqc;

							MyTextureP = &MyTexturePP[int(scaleCnt)];
							if(MyTextureP->r + MyTextureP->b + MyTextureP->b)
							{
							// Video Cast
								alpha = static_cast<float>(MyTextureP->a)*0.00392f;
								alphaInv = 1 - alpha;
								g_video_buffer->r = static_cast<BYTE>(MyTextureP->r * objalpha + g_video_buffer->r * objalphaInv);
								g_video_buffer->g = static_cast<BYTE>(MyTextureP->g * objalpha + g_video_buffer->g * objalphaInv);
								g_video_buffer->b = static_cast<BYTE>(MyTextureP->b * objalpha + g_video_buffer->b * objalphaInv);
							}
						//}
						g_video_buffer++;
						scaleCnt+=scaleStep;
						pZBuffer++;
					}

				} else {

					for(x = sprite_sl; x < sprite_sw;x++)
					{
						if((qqqc = obj->l.z+g_zbuffer_page) > pZBuffer[0])
						{
							MyTextureP = &MyTexturePP[int(scaleCnt)];
							if(MyTextureP->r + MyTextureP->g + MyTextureP->b)
							{
								pZBuffer[0] = qqqc;
								*g_video_buffer = *MyTextureP;
							}
						}
						g_video_buffer++;
						scaleCnt+=scaleStep;
						pZBuffer++;
					}

				}


			}

			textureVpos += textureVstep;
			g_video_buffer += video_rx;
			pZBuffer += video_rxz;
		}

		g_rendered_sprites += 1;

	}
//debug1 = SDL_GetTicks()-debug1;
//debug1 = SDL_GetTicks() - debug1;
//if(debug1) debug1 = debug1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rotate the world poly set and lights about CamX,CamY,CamZ
//	return new world as Obt & LSt
//	perform basic culling.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void rotate_world(void)
{	
	z_size_t a=0,b=0;
	//object_type *curobj;
	Vertex *curvertex;


	//float globalMatrix[4][4];
	Matrix44 globalMatrix;
	globalMatrix.ident();

	Vec3 ta = { -g_camera_angle.x, g_camera_angle.y, g_camera_angle.z };
	globalMatrix.rotate(ta);
	
	Matrix44 obMatrix;

	float zover;

	for(a=0;a<g_world.obj.size();a++)
	{
		
		curobj = g_world.obj[a].get();

		// Copy vert .. obj center
		curobj->wposition = curobj->position;

		obMatrix.ident();
		if (a == 101) {
			int a = 12;
		}

		obMatrix.rotate(curobj->angle);
		obMatrix.translate(curobj->position);

		for(b=0;b<curobj->vertcount;b++)
		{
			curvertex = &curobj->vertex[b];

			// Copy & Scale vert
			curvertex->w = curvertex->l * curobj->scale;
			// Rotate by object
			obMatrix.transform(curvertex->w);

			// Rotate & translate to camera.
			curvertex->w= curvertex->w - g_camera_position;

			globalMatrix.transform(curvertex->w);
		}

		// Translate & Rotate & Project object center into screen space.
		curobj->wposition = curobj->wposition - g_camera_position;
		globalMatrix.transform(curobj->wposition);
		zover = (1/curobj->wposition.z) * g_view_distance;
		curobj->wposition.x = (curobj->wposition.x * zover) + SCREEN_WIDTHh;
		curobj->wposition.y = (curobj->wposition.y * zover) + SCREEN_HEIGHTh;

	}
	
	// Transform Sprites./////////////////////////////////////

	Sprite *sprite_tmp;

	for(a = 0;a < sprites_back_cnt;a++)
	{
		sprite_tmp = &sprites_back[a];
		if(sprite_tmp->distance_infinity)
		{
			sprite_tmp->l = sprite_tmp->position;
		} else {
			sprite_tmp->l = sprite_tmp->position - g_camera_position;
		}

		globalMatrix.transform(sprite_tmp->l);
	}
	for(a = 0;a < sprites_front_cnt;a++)
	{
		sprite_tmp = &sprites_front[a];
		sprite_tmp->l = sprite_tmp->position - g_camera_position;
		globalMatrix.transform(sprite_tmp->l);
	}

	// Cull some of the 3d mesh's
	
	int cntobs=0;
	float tsc;
	for(a=0;a<g_world.obj.size();a++)
	{

		curobj = g_world.obj[a].get();
		tsc = curobj->scale * 50;
		if(curobj->wposition.y - tsc < SCREEN_HEIGHT && curobj->wposition.y + tsc  > 0 && curobj->wposition.x  - tsc < SCREEN_WIDTH && curobj->wposition.x + tsc > 0)
		{
			//debug3++;

			
			
			for(b=0;b<curobj->polycount;b++)
			{
				if (curobj->hidden == true) curobj->polygon[b].visable = 0;
				if(curobj->polygon[b].visable==0)
				{
					curobj->polygon[b].compute_normal();
					if(curobj->polygon[b].D < -0.2 ) curobj->polygon[b].visable = static_cast<unsigned int>(5 * -curobj->polygon[b].D);
				} else curobj->polygon[b].visable--;
			}
			curobj->hidden = false;

		} else {
			curobj->hidden = true;
		}
	}

	//debug3 = WLD.obj[0].wy ;

}


/*
//https://en.wikipedia.org/wiki/Subnormal_number
#define _MM_DENORMALS_ZERO_MASK   0x0040
#define _MM_DENORMALS_ZERO_ON     0x0040
#define _MM_DENORMALS_DAZ_FTZ_ON  0x8040
#define _MM_DENORMALS_ZERO_OFF    0x0000

#define _MM_SET_DENORMALS_ZERO_MODE(mode) _mm_setcsr((_mm_getcsr() & ~_MM_DENORMALS_ZERO_MASK) | (mode))
#define _MM_GET_DENORMALS_ZERO_MODE()                (_mm_getcsr() &  _MM_DENORMALS_ZERO_MASK)
*/

inline void mesh_lights()
{
	//_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_DAZ_FTZ_ON);
	//
	//	Load and set up App Objects and Lights
	//

	pclip.vertcount = 0;
	LX xloader;

	xloader.loadFile(L"assets\\fighter.obj");
	
	for(int i=0;i<mines_cnt;i++)
		xloader.loadFile(L"assets\\mine.obj");

	xloader.loadFile(L"assets\\earth.obj");

	xloader.cleanUp();


	long vertCnt=0;
	long faceCnt=0;

	for (z_size_t i=0; i<xloader.getObjectCount(); i++)
	{
		Mesh* curmesh = xloader.getObject(i);
		z_size_t attrOffset = xloader.getAttrOffset(i);
		auto curobj = std::make_unique<ZSpace_Object>();

		curobj->thrust = 0;
		curobj->v = { 0,0,0 };
		curobj->angle = { 0,0,0 };
		curobj->position = { 0,0,0 };
		

		//curobj.vertcount = xloader.getObject(i).GetVertexCount();	// Number of verts in this object?
		curobj->vertcount = curmesh->mnVerts;		// Number of verts in this object?
		curobj->vertex = new Vertex[curobj->vertcount];	// Allocate memory for vertex array;

		for(z_size_t v=0; v<curobj->vertcount; v++)
		{
			Vertex *curvert=&curobj->vertex[v];
			curvert->l.x = curmesh->mPositions[v].x;
			curvert->l.y = -curmesh->mPositions[v].z;	// was - .z
			curvert->l.z = curmesh->mPositions[v].y; // was .y
			vertCnt++;
		}

		curobj->polycount = curmesh->mnFaces;	// Set number of polygons in object
		curobj->polygon = new Poly[curobj->polycount];


		for(z_size_t p=0; p<curobj->polycount; p++)
		{
			Poly *curpoly = &curobj->polygon[p];

			// ********** NOT needed now since vertex count is always = 3
			//curpoly->vertcount = 3;
			// Assign memory for vertex array
			//curpoly->vertex = new vertex_type*[curpoly->vertcount];
			//curpoly->uv = new UV[curpoly->vertcount];
			// Create an array of pointers to verticies.
			// Allocate memory for pointer and point it at vertex in vertex array;
			//mIndices[face * 3 + point] + 1
			curpoly->vertex[0] = &curobj->vertex[curmesh->mIndices[p * 3 + 0]];
			curpoly->vertex[1] = &curobj->vertex[curmesh->mIndices[p * 3 + 1]];
			curpoly->vertex[2] = &curobj->vertex[curmesh->mIndices[p * 3 + 2]];

			if (curmesh->mTexCoords) {
				curpoly->uv[0].u = curmesh->mTexCoords[curmesh->mIndices[p * 3 + 0]].x;
				curpoly->uv[0].v = curmesh->mTexCoords[curmesh->mIndices[p * 3 + 0]].y;
				curpoly->uv[1].u = curmesh->mTexCoords[curmesh->mIndices[p * 3 + 1]].x;
				curpoly->uv[1].v = curmesh->mTexCoords[curmesh->mIndices[p * 3 + 1]].y;
				curpoly->uv[2].u = curmesh->mTexCoords[curmesh->mIndices[p * 3 + 2]].x;
				curpoly->uv[2].v = curmesh->mTexCoords[curmesh->mIndices[p * 3 + 2]].y;
				for (int b = 0; b < 3; b++)
				{
					if (curpoly->uv[b].u < 0) curpoly->uv[b].u = 0;
					if (abs(curpoly->uv[b].u) < 0.000000001) curpoly->uv[b].u = 0;
					if (curpoly->uv[b].v < 0) curpoly->uv[b].v = 0;
					if (abs(curpoly->uv[b].v) < 0.000000001) curpoly->uv[b].v = 0;
				}
			} else {
				curpoly->uv[2].u = 0;
				curpoly->uv[2].v = 0;
				curpoly->uv[1].u = 0;
				curpoly->uv[1].v = 0;
				curpoly->uv[0].u = 0;
				curpoly->uv[0].v = 0;
			}

			curpoly->SurfaceTexture = attrOffset + curmesh->mAttributes[p]; // xloader.GetObject(i).GetPolygon(p).SurfaceTexture;;
			curpoly->visable=0;
			faceCnt++;
		}

		g_world.obj.push_back(std::move(curobj));

	}

	dbg(L"count %d", xloader.getTextureCount());

	int tmpmapcnt=0;
	for(z_size_t i = 0; i < xloader.getTextureCount(); i++)
	{
		bool loadmap = true;
		Mesh::Material* curtexture = xloader.getTexture(i);

		BYTE faceR = 0, faceG = 0, faceB = 0;
		Texture texture;
		texture.mat_diffuse.r = curtexture->diffuseColor.x * 255.0f;
		texture.mat_diffuse.g = curtexture->diffuseColor.y * 255.0f;
		texture.mat_diffuse.b = curtexture->diffuseColor.z * 255.0f;
		

		if(curtexture->texture.length() > 0)
		{

			for(z_size_t p=0; p<i; p++)
			{
				if(p!=i)
				{
					if(xloader.getTexture(p)->texture.compare(curtexture->texture)==0)
					//if(strcmp(xloader.GetTexture(i).bmFileName, Texture[p].filename))
					{
						for (z_size_t ob = 0; ob < g_world.obj.size(); ob++)
						{
							for (z_size_t v = 0; v < g_world.obj[ob]->polycount; v++)
								if (g_world.obj[ob]->polygon[v].SurfaceTexture == i)
								{
									g_world.obj[ob]->polygon[v].SurfaceTexture = p;
								}
						}
						loadmap = false;
						break;
					}
				}
			}

			if(loadmap)
			{
				std::string str;
				std::transform(curtexture->texture.begin(), curtexture->texture.end(), std::back_inserter(str), [](wchar_t c) {
					return (char)c;
				});

				load_texture(texture, str);
				tmpmapcnt++;
			}
		}

		g_textures.push_back(texture);


	}
	

	xloader.clear();
}



bool init_world(void)
{

	mesh_lights();
	
	Texture texture;
	load_texture(texture, "star2.bmp");
	g_textures.push_back(texture);
	load_texture(texture, "earth.bmp");
	g_textures.push_back(texture);

	//load_texture_alpha(g_textures[g_textures_cnt-1], L"earth_alpha.bmp",false);
	
	sprites_back_cnt = 500;
	sprites_back = new Sprite[sprites_back_cnt];
	
	float an = 0;
	float anb = 0;
	for(z_size_t tmp=0;tmp < sprites_back_cnt;tmp++)
	{
		an = static_cast<float>(rand()%359);
		anb= static_cast<float>(rand()%359);
		dbg(L"COS %f", MyCos(an));
		sprites_back[tmp].position.x =  MyCos(anb) * MyCos(an) * 20000;
		sprites_back[tmp].position.z =  MyCos(anb) * MySin(an) * 20000;
		sprites_back[tmp].position.y =  MySin(anb) * 20000;
		sprites_back[tmp].v.x = 0;
		sprites_back[tmp].v.y = 0;
		sprites_back[tmp].v.z = 0;
		sprites_back[tmp].p = true;
		sprites_back[tmp].boTexture = g_textures.size() - 2;
		sprites_back[tmp].visable = true;
		sprites_back[tmp].distance_infinity = true;
		sprites_back[tmp].zbuffer = false;
		sprites_back[tmp].scale = false;
		sprites_back[tmp].alpha = 1;
	}
	sprites_back[sprites_back_cnt-2].position.x =  20000;
	sprites_back[sprites_back_cnt-2].position.z =  10000;
	sprites_back[sprites_back_cnt-2].position.y =  20000;
	sprites_back[sprites_back_cnt-2].p = true;
	sprites_back[sprites_back_cnt-2].boTexture = g_textures.size()-1;
	sprites_back[sprites_back_cnt-2].visable = true;
	sprites_back[sprites_back_cnt-2].distance_infinity = false;
	sprites_back[sprites_back_cnt-2].zbuffer = false;
	sprites_back[sprites_back_cnt-2].scale = false;
	sprites_back[sprites_back_cnt-2].alpha = 1;

	sprites_front_cnt=5000;
	sprites_front = new Sprite[sprites_front_cnt];	

	
	load_texture(texture, "shot_00.bmp");
	g_textures.push_back(texture);
	load_texture(texture, "shot_01.bmp");
	g_textures.push_back(texture);
	load_texture(texture, "shot_02.bmp");
	g_textures.push_back(texture);
	
	Texture* t = &g_textures.back();
	Colour<BYTE>* b = t->pixels_colour;

	for (int a = 0; a < 9; a++)
	{
		dbg(L"%i %i %i %i\r\n", b[0], b[1], b[2], b[3]);
		b += 4;
	}

	for(z_size_t tmp=0;tmp < sprites_front_cnt;tmp++)
	{
		sprites_front[tmp].position = { 0, 0, static_cast<float>(tmp) };
		sprites_front[tmp].v = { 0,0,0 };
		sprites_front[tmp].p = true;
		sprites_front[tmp].liveTime = 0;
		sprites_front[tmp].visable = false;
		sprites_front[tmp].distance_infinity = false;
		sprites_front[tmp].zbuffer = true;
		sprites_front[tmp].scale = true;
		sprites_front[tmp].oscale = 0.3f;
		sprites_front[tmp].animation_len = 1;
		//sprites_front[tmp].animation_len = TextureCnt-1;
		sprites_front[tmp].boTexture = g_textures.size() - 1;
		sprites_front[tmp].alpha = 0.5;
	}
	g_shoot_sprite_idx = g_textures.size() - 3;
	g_shot_sprite_len = 3;


	
	for(unsigned int tmp=0 ; tmp<62 ; tmp+=2)
	{
		std::string tbuffer = std::format("explosion\\explosion{:04}.bmp", tmp);
		load_texture(texture,tbuffer);
		g_textures.push_back(texture);
		Texture diffuse_texture = g_textures.back();

		Texture alpha_texture;
		tbuffer = std::format("explosion\\explosion_Alpha{:04}.bmp", tmp);
		load_texture(alpha_texture, tbuffer.c_str());
		//g_textures.push_back(alpha_texture);

		Colour<BYTE>* src = diffuse_texture.pixels_colour;
		Colour<BYTE>* dst = alpha_texture.pixels_colour;
		for (int a = 0; a < diffuse_texture.bmWidth * diffuse_texture.bmHeight; a++)
		{
			dst->a = src->r;

		}


	}
	load_texture(texture, "shrap.bmp");
	g_textures.push_back(texture);
	g_shrapnel_sprite_idx = g_textures.size()-1;
	g_shrapnel_sprite_len = 1;

	// Set up forground sprites


	g_boom_sprite_idx = g_textures.size()-32;
	g_boom_sprite_len = 32;

	// Set up asteroids.
	int ststart = 1;
	//int an; 
	for(z_size_t tmp=ststart;tmp<mines_cnt - ststart;tmp++)
	{
		//WLD.obj[tmp].x = rand()%1500 - 750;
		//WLD.obj[tmp].z = rand()%750-100;
		g_world.obj[tmp]->position.x = (float)(rand()%1000 - 500);
		g_world.obj[tmp]->position.z = (float)(rand()%1000 - 500);
		g_world.obj[tmp]->position.y = 0; // (float)(rand() % 8 - 4);

		an = static_cast<float>(rand()%360-180);
		g_world.obj[tmp]->va.x = float(((rand()%10)-5))/5;//((MyCos(an) * 10)-5)/4;
		g_world.obj[tmp]->va.y = 0;
		g_world.obj[tmp]->va.z = float(((rand()%10)-5))/5;//((MyCos(an) * 10)-5)/4;
		g_world.obj[tmp]->va.x = float(rand() % 10) / 10;
		g_world.obj[tmp]->va.z = float(rand() % 10) / 10;
		an = static_cast<float>(rand()%359);
		anb = static_cast<float>(rand()%10);
		g_world.obj[tmp]->v.x =  (MyCos(an)/10 * anb);
		g_world.obj[tmp]->v.y =  0; //(rand()%2 - 1);
		g_world.obj[tmp]->v.z =  (MySin(an)/10 * anb);

		g_world.obj[tmp]->scale = float(rand()%5+1);
		g_world.obj[tmp]->mass = g_world.obj[tmp]->scale;
		g_world.obj[tmp]->bounds = g_world.obj[tmp]->mass * 5;
	}
	
	g_world.obj[ststart]->position.x = -100;
	g_world.obj[ststart]->position.y = -100;
	g_world.obj[ststart]->position.z = 0;
	g_world.obj[ststart]->va.x = 1;
	g_world.obj[ststart]->va.y = 1;
	g_world.obj[ststart]->va.z = 1;
	g_world.obj[ststart]->v.x = 0;
	g_world.obj[ststart]->v.y = 0;
	g_world.obj[ststart]->v.z = 0;


	g_camera_position = { 0,0,0 };
	g_camera_angle = { -50,0,0 };
	g_cam_object_idx = 0;
	camobj = g_world.obj[g_cam_object_idx].get();
	camobj->position = { 0,0,0 };
	camobj->scale = 1;
	camobj->mass = 5;
	camobj->bounds = 5;
	g_cam_zoom_y = -150;

	rotate_world();

	return true;
}


void deinit_world(void)
{
	delete [] sprites_back;
	delete [] sprites_front;

	for(auto texture : g_textures)
	{
		if (texture.pixels_normal) SDL_free(texture.pixels_normal);
	}
	
	for(z_size_t a=0;a < g_world.obj.size();a++)
	{
		delete [] g_world.obj[a]->polygon;
		delete [] g_world.obj[a]->vertex;
	
	}
}





unsigned int polyx[SCREEN_HEIGHT][2];
float polyz[SCREEN_HEIGHT][2];
float polyuv[SCREEN_HEIGHT][4];




void z_clip(Poly *polygon,Clipped_Poly *clip)
{
	float t;
	Clip *pcv=clip->vertex;

	int cp=0;
	float zmin=2;	// don't go below zero.

	int v1=2;
	
	Vertex *pv1;
	Vertex *pv2;
	UV *puv1;
	UV *puv2;

	for(int v2=0; v2<3; v2++)
	{
		pv1=polygon->vertex[v1];
		pv2=polygon->vertex[v2];

		puv1=&polygon->uv[v1];
		puv2=&polygon->uv[v2];

		if((pv1->w.z >= zmin) && (pv2->w.z >= zmin))
		{
			
			pcv[cp].position.x = pv2->w.x;
			pcv[cp].position.y = pv2->w.y;
			pcv[cp].u = puv2->u;
			pcv[cp].v = puv2->v;
			pcv[cp++].position.z = pv2->w.z;
		}
		if((pv1->w.z >= zmin) && (pv2->w.z < zmin))
		{
			t=(zmin - pv1->w.z) / (pv2->w.z - pv1->w.z);
			pcv[cp].position.x = pv1->w.x + (pv2->w.x - pv1->w.x) * t;
			pcv[cp].position.y = pv1->w.y + (pv2->w.y - pv1->w.y) * t;
			pcv[cp].u = puv1->u  + (puv2->u  - puv1->u) * t;
			pcv[cp].v = puv1->v  + (puv2->v  - puv1->v) * t;
			pcv[cp++].position.z = zmin;

		}
		if((pv1->w.z < zmin) && (pv2->w.z >= zmin))
		{
			t=(zmin - pv1->w.z) / (pv2->w.z - pv1->w.z);
			pcv[cp].position.x = pv1->w.x + (pv2->w.x - pv1->w.x) * t;
			pcv[cp].position.y = pv1->w.y + (pv2->w.y - pv1->w.y) * t;
			pcv[cp].u = puv1->u + (puv2->u - puv1->u) * t;
			pcv[cp].v = puv1->v + (puv2->v - puv1->v) * t;
			pcv[cp++].position.z = zmin;
			pcv[cp].position.x = pv2->w.x;
			pcv[cp].position.y = pv2->w.y;
			pcv[cp].u = puv2->u;
			pcv[cp].v = puv2->v;
			pcv[cp++].position.z = pv2->w.z;
		}
		v1 = v2;
	}

	clip->vertcount = cp;
}




void t_clip(Clipped_Poly *clip)
{
	Clip *pcv=clip->vertex;
	Clip *pv1;
	Clip *pv2;

	float t;
	float ymin=1;
	float ymax=SCREEN_HEIGHT-1;
	float xmin=1;
	float xmax=SCREEN_WIDTH-1;

	// Clip to screen top at ymin
	int cp=0;
	int v1=clip->vertcount-1;

	for(z_size_t v2=0; v2 < clip->vertcount; v2++)
	{
		pv1=&clip->vertex[v1];
		pv2=&clip->vertex[v2];
		if((pv1->y1 >= ymin) && (pv2->y1 >= ymin))
		{
			pcv[cp].position.z = pv2->z1;
			pcv[cp].position.x = pv2->x1;
			pcv[cp].u = pv2->u1;
			pcv[cp].v = pv2->v1;
			pcv[cp++].position.y = pv2->y1;
		}
		if((pv1->y1 >= ymin) && (pv2->y1 < ymin))
		{
			t=(ymin - pv1->y1) / (pv2->y1 - pv1->y1);
			pcv[cp].position.x = pv1->x1 + (pv2->x1 - pv1->x1) * t;
			pcv[cp].position.z = pv1->z1 + (pv2->z1 - pv1->z1) * t;
			pcv[cp].u = pv1->u1 + (pv2->u1 - pv1->u1) * t;
			pcv[cp].v = pv1->v1 + (pv2->v1 - pv1->v1) * t;
			pcv[cp++].position.y = ymin;
		}
		if((pv1->y1 < ymin) && (pv2->y1 >= ymin))
		{
			t=(ymin - pv1->y1) / (pv2->y1 - pv1->y1);
			pcv[cp].position.x = pv1->x1 + (pv2->x1 - pv1->x1) * t;
			pcv[cp].position.z = pv1->z1 + (pv2->z1 - pv1->z1) * t;
			pcv[cp].u = pv1->u1 + (pv2->u1 - pv1->u1) * t;
			pcv[cp].v = pv1->v1 + (pv2->v1 - pv1->v1) * t;
			pcv[cp++].position.y = ymin;
			pcv[cp].position.x = pv2->x1;
			pcv[cp].position.z = pv2->z1;
			pcv[cp].u = pv2->u1;
			pcv[cp].v = pv2->v1;
			pcv[cp++].position.y = pv2->y1;
		}
		v1 = v2;
	}
	clip->vertcount = cp;

	// Clip to screen bottom at ymax
	cp=0;
	v1=clip->vertcount-1;
	for(z_size_t v2=0; v2 < clip->vertcount; v2++)
	{
		pv1=&clip->vertex[v1];
		pv2=&clip->vertex[v2];
		if((pv1->position.y <= ymax) && (pv2->position.y <= ymax))
		{
			pcv[cp].x1 = pv2->position.x;
			pcv[cp].z1 = pv2->position.z;
			pcv[cp].u1 = pv2->u;
			pcv[cp].v1 = pv2->v;
			pcv[cp++].y1 = pv2->position.y;
		}
		if((pv1->position.y <= ymax) && (pv2->position.y > ymax))
		{
			t=(ymax - pv1->position.y) / (pv2->position.y - pv1->position.y);
			pcv[cp].x1 = pv1->position.x + (pv2->position.x - pv1->position.x) * t;
			pcv[cp].z1 = pv1->position.z + (pv2->position.z - pv1->position.z) * t;
			pcv[cp].u1 = pv1->u + (pv2->u - pv1->u) * t;
			pcv[cp].v1 = pv1->v + (pv2->v - pv1->v) * t;
			pcv[cp++].y1 = ymax;
		}
		if((pv1->position.y > ymax) && (pv2->position.y <= ymax))
		{
			t=(ymax - pv1->position.y) / (pv2->position.y - pv1->position.y);
			pcv[cp].x1 = pv1->position.x + (pv2->position.x - pv1->position.x) * t;
			pcv[cp].z1 = pv1->position.z + (pv2->position.z - pv1->position.z) * t;
			pcv[cp].u1 = pv1->u + (pv2->u - pv1->u) * t;
			pcv[cp].v1 = pv1->v + (pv2->v - pv1->v) * t;
			pcv[cp++].y1 = ymax;
			pcv[cp].x1 = pv2->position.x;
			pcv[cp].z1 = pv2->position.z;
			pcv[cp].u1 = pv2->u;
			pcv[cp].v1 = pv2->v;
			pcv[cp++].y1 = pv2->position.y;
		}
		v1 = v2;
	}
	clip->vertcount = cp;

	// Clip to screen left at xmin
	cp=0;
	v1=clip->vertcount-1;

	for(z_size_t v2=0; v2 < clip->vertcount; v2++)
	{
		pv1=&clip->vertex[v1];
		pv2=&clip->vertex[v2];
		if((pv1->x1 >= xmin) && (pv2->x1 >= xmin))
		{
			pcv[cp].position.y = pv2->y1;
			pcv[cp].position.z = pv2->z1;
			pcv[cp].u = pv2->u1;
			pcv[cp].v = pv2->v1;
			pcv[cp++].position.x = pv2->x1;
		}
		if((pv1->x1 >= xmin) && (pv2->x1 < xmin))
		{
			t=(xmin - pv1->x1) / (pv2->x1 - pv1->x1);
			pcv[cp].position.y = pv1->y1 + (pv2->y1 - pv1->y1) * t;
			pcv[cp].position.z = pv1->z1 + (pv2->z1 - pv1->z1) * t;
			pcv[cp].v = pv1->v1 + (pv2->v1 - pv1->v1) * t;
			pcv[cp].u = pv1->u1 + (pv2->u1 - pv1->u1) * t;
			pcv[cp++].position.x = xmin;
		}
		if((pv1->x1 < xmin) && (pv2->x1 >= xmin))
		{
			t=(xmin - pv1->x1) / (pv2->x1 - pv1->x1);
			pcv[cp].position.y = pv1->y1 + (pv2->y1 - pv1->y1) * t;
			pcv[cp].position.z = pv1->z1 + (pv2->z1 - pv1->z1) * t;
			pcv[cp].v = pv1->v1 + (pv2->v1 - pv1->v1) * t;
			pcv[cp].u = pv1->u1 + (pv2->u1 - pv1->u1) * t;
			pcv[cp++].position.x = xmin;
			pcv[cp].position.y = pv2->y1;
			pcv[cp].position.z = pv2->z1;
			pcv[cp].u = pv2->u1;
			pcv[cp].v = pv2->v1;
			pcv[cp++].position.x = pv2->x1;
		}
		v1 = v2;
	}
	clip->vertcount = cp;

	// Clip to screen right at xmax
	cp=0;
	v1=clip->vertcount-1;
	for(z_size_t v2=0; v2 < clip->vertcount; v2++)
	{
		pv1=&clip->vertex[v1];
		pv2=&clip->vertex[v2];
		if((pv1->position.x <= xmax) && (pv2->position.x <= xmax))
		{
			pcv[cp].y1 = pv2->position.y;
			pcv[cp].z1 = pv2->position.z;
			pcv[cp].u1 = pv2->u;
			pcv[cp].v1 = pv2->v;
			pcv[cp++].x1 = pv2->position.x;
		}
		if((pv1->position.x <= xmax) && (pv2->position.x > xmax))
		{
			t=(xmax - pv1->position.x) / (pv2->position.x - pv1->position.x);
			pcv[cp].y1 = pv1->position.y + (pv2->position.y - pv1->position.y) * t;
			pcv[cp].z1 = pv1->position.z + (pv2->position.z - pv1->position.z) * t;
			pcv[cp].u1 = pv1->u + (pv2->u - pv1->u) * t;
			pcv[cp].v1 = pv1->v + (pv2->v - pv1->v) * t;
			pcv[cp++].x1 = xmax;
		}
		if((pv1->position.x > xmax) && (pv2->position.x <= xmax))
		{
			t=(xmax - pv1->position.x) / (pv2->position.x - pv1->position.x);
			pcv[cp].y1 = pv1->position.y + (pv2->position.y - pv1->position.y) * t;
			pcv[cp].z1 = pv1->position.z + (pv2->position.z - pv1->position.z) * t;
			pcv[cp].u1 = pv1->u + (pv2->u - pv1->u) * t;
			pcv[cp].v1 = pv1->v + (pv2->v - pv1->v) * t;
			pcv[cp++].x1 = xmax;
			pcv[cp].y1 = pv2->position.y;
			pcv[cp].z1 = pv2->position.z;
			pcv[cp].u1 = pv2->u;
			pcv[cp].v1 = pv2->v;
			pcv[cp++].x1 = pv2->position.x;
		}
		v1 = v2;
	}

	clip->vertcount = cp;

}



inline void render_tf()
{
	
	float SurfaceMultW;
	float SurfaceMultH;


	Colour<BYTE>* dibits;

	BYTE x,y;
	unsigned int i;

	float zover;
	float qqqc;

	bool hasmap;

	Colour<BYTE> faceC = {};

	Texture *texture;
	Colour<BYTE> *MyTextureP = NULL;
	// Reasterizer vars
	unsigned int a=0;

	// Daves Rasterizer.
	z_size_t vertex_cnt;
	unsigned int top_vr,bot_vr,top_vl,BotVl;
	int top_yr,bot_yr,TopYl,BotYl;
	float distance_r;
	float distance_l;
	float t_value;
	float step_u,step_v,step_zz;
	float step_ul,step_vl,step_zl,step_xl;
	float step_ur,step_vr,step_zr,step_xr;
	float xl,xr;
	float zl,ul,vl,zr,ur,vr;

	float tz,tu,tv;
	float delta_x;
	float one_over_z;

	z_size_t t;

	z_size_t poly_cnt;
	z_size_t obj_cnt;
	ZSpace_Object *curobj;
	Poly *curpolygon;


	// Clear ZBuffer if needed 

	//BYTE*					video_bufferC		= NULL;	// used to draw onto back-keyboard_buffer
	//byte video_local[SCREEN_WIDTH*SCREEN_HEIGHT];
	//ZeroMemory ( video_local, sizeof ( video_local ));
	//video_bufferC = video_local;
	//delete video_local;

	// For every object in the World
	for(obj_cnt=0;obj_cnt<g_world.obj.size();obj_cnt++)
	{

		curobj = g_world.obj[obj_cnt].get();	// Pointer to this object.
		if(curobj->hidden) continue;
		if(curobj->scale==0) continue;
		// For every polygon in an object
		g_total_faces += curobj->polycount;
		for(poly_cnt=0;poly_cnt<curobj->polycount;poly_cnt++)
		{
			curpolygon = &curobj->polygon[poly_cnt];// Pointer to this polygon.

			if(curpolygon->visable>0) continue;	// Calculated in rotate world. >0 indicates not visable.

			if (obj_cnt == 1) {
				int a = 0;
			}

			z_clip(curpolygon,&pclip);		// Clip polygon to frontZ and return pclip so user does not get poked in the eye!
			if(pclip.vertcount<3) continue;	// No points left in polygon so move on to next



			for(t=0; t<pclip.vertcount; t++) {  // Project verts to screen space.
				zover = 1/pclip.vertex[t].position.z;	// zclip will ensure z is never 0
				pclip.vertex[t].x1 = (pclip.vertex[t].position.x * g_view_distance * zover) + SCREEN_WIDTHh;
				pclip.vertex[t].y1 = (pclip.vertex[t].position.y * g_view_distance * zover) + SCREEN_HEIGHTh;
				pclip.vertex[t].z1 = zover;
				pclip.vertex[t].u1 = pclip.vertex[t].u * zover;	// uv texture cords.
				pclip.vertex[t].v1 = pclip.vertex[t].v * zover;

			}
			
			t_clip(&pclip);	// Clip polygon against edges of the screen.

			if(pclip.vertcount<3) continue;	// No points left in polygon so move on to next

			// Get Texture details

			texture = &g_textures[curpolygon->SurfaceTexture];
			if(texture->bmWidth)
			{
				hasmap = true;
				MyTextureP   = texture->pixels_colour;
				SurfaceMultH = static_cast<float>(texture->bmHeight);
				SurfaceMultW = static_cast<float>(texture->bmWidth);
			} else {
				hasmap = false;
				faceC.r = static_cast<BYTE>(texture->mat_diffuse.r * curpolygon->D);
				faceC.g = static_cast<BYTE>(texture->mat_diffuse.g * curpolygon->D);
				faceC.b = static_cast<BYTE>(texture->mat_diffuse.b * curpolygon->D);
			}

			
			// Rasterize.

			top_vr = 0 ; top_yr = static_cast<int>(pclip.vertex[0].y1);
			vertex_cnt = pclip.vertcount;
			// Find top vertex
			for(a = 0; a<vertex_cnt; a++)
			{
				if(pclip.vertex[a].y1 < top_yr)
				{
					top_vr = a; top_yr = static_cast<int>(pclip.vertex[a].y1);
				}
			}

			top_vl = top_vr + 1; top_vr = top_vr - 1;

			distance_r = 0;
			distance_l = 0;

		    while(1)
			{

				if(!distance_r)
				{
				NewR:
					if(!vertex_cnt--) break;
					top_vr++;
					if(top_vr == pclip.vertcount) top_vr = 0;
					if(top_vr == pclip.vertcount-1) bot_vr = 0; else bot_vr = top_vr + 1;

					top_yr = static_cast<int>(pclip.vertex[top_vr].y1);
					bot_yr = static_cast<int>(pclip.vertex[bot_vr].y1);

					distance_r = static_cast<float>(top_yr - bot_yr);
					if(!distance_r) goto NewR;

					xr = pclip.vertex[top_vr].x1;
					zr = pclip.vertex[top_vr].z1;
					ur = pclip.vertex[top_vr].u1;
					vr = pclip.vertex[top_vr].v1;

					t_value = 1 / distance_r;
					step_xr = (xr - pclip.vertex[bot_vr].x1) * t_value;
					step_zr = (zr - pclip.vertex[bot_vr].z1) * t_value;
					step_ur = (ur - pclip.vertex[bot_vr].u1) * t_value;
					step_vr = (vr - pclip.vertex[bot_vr].v1) * t_value;
				}


				if(!distance_l)
				{
				newL:
					if(!vertex_cnt--) break;
					top_vl--;
					if(top_vl==-1) top_vl = pclip.vertcount - 1;
					if(top_vl==0) BotVl = pclip.vertcount - 1; else BotVl = top_vl - 1;
					
					TopYl = static_cast<int>(pclip.vertex[top_vl].y1);
					BotYl = static_cast<int>(pclip.vertex[BotVl].y1);

					distance_l = static_cast<float>(TopYl - BotYl);
					if(!distance_l) goto newL;

					xl = pclip.vertex[top_vl].x1+1;
					zl = pclip.vertex[top_vl].z1;
					ul = pclip.vertex[top_vl].u1;
					vl = pclip.vertex[top_vl].v1;

					t_value = 1 / distance_l;
					step_xl = (xl - pclip.vertex[BotVl].x1) * t_value;
					step_zl = (zl - pclip.vertex[BotVl].z1) * t_value;
					step_ul = (ul - pclip.vertex[BotVl].u1) * t_value;
					step_vl = (vl - pclip.vertex[BotVl].v1) * t_value;
				}
        
				//////////////////////////////////////
		        
				if(delta_x = xr - xl+1)
				{
					delta_x = 1/delta_x;
					step_zz = (zr - zl) * delta_x;

					tz = zl;

					g_video_buffer = g_video_bufferB + int(xl) + (top_yr * SCREEN_WIDTH);
					pZBuffer = ZBuffer + int(xl) + (top_yr*SCREEN_WIDTH);	

					if(hasmap)
					{

						step_u = (ur - ul) * delta_x * SurfaceMultW;
						step_v = (vr - vl) * delta_x * SurfaceMultH;

						tu = ul*SurfaceMultW;
						tv = vl*SurfaceMultH;

						for(i = xr; i>xl-1;--i)
						//for(i = Xl; i<Xr;i++)
						{
							if((qqqc = tz+g_zbuffer_page) > *pZBuffer)
							{
								*pZBuffer = qqqc;
								one_over_z = 1 / tz;
								x = static_cast<BYTE>(tu * one_over_z);
								y = static_cast<BYTE>(tv * one_over_z);
								dibits = &MyTextureP[ (long)x + ((long)y * texture->bmWidth)];
								g_video_buffer->r = static_cast<BYTE>(dibits->r * curpolygon->D);
								g_video_buffer->g = static_cast<BYTE>(dibits->g * curpolygon->D);
								g_video_buffer->b = static_cast<BYTE>(dibits->b * curpolygon->D);
							}
							tz+=step_zz;
							tu+=step_u;
							tv+=step_v;

							g_video_buffer++;
							pZBuffer++;
						}

					} else {

						for(i = xr; i>xl-1;--i)
						{
							if((qqqc = tz+g_zbuffer_page) > *pZBuffer)
							{
								*pZBuffer = qqqc;
								*g_video_buffer = faceC;	// SDL -> 0xAABBGGRR

							}

							tz+=step_zz;
							g_video_buffer++;
							pZBuffer++;
						}

					}

				}

				//////////////////////////////////////
				xr+=step_xr;
				zr+=step_zr;
				ur+=step_ur;
				vr+=step_vr;

				xl+=step_xl;
				zl+=step_zl;
				ul+=step_ul;
				vl+=step_vl;
        
				distance_r++;
				distance_l++;
				top_yr++;
				TopYl++;
			}
			g_rendered_faces += 1;
		}

		g_rendered_objects += 1;
	}
     
	//debug2 = SDL_GetTicks()-debug2;
}







