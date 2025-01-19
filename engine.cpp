/*
	 zSpace code.
	 Daves 3D Engine. 

Copyright (c) 2002-2006, David Chamberlain - DaveZ@204am.com

*/

#define WIN32_LEAN_AND_MEAN
#define INITGUID

#include <windows.h>
#include <format>
#include <SDL3/SDL.h>
#include "world.h"
#include "engine.h"
#include "engine_types.h"
#include "math.h"
//#include "lx.h"	// Cannot remember what library this was from so I made my own LX class.


void ProcessInput(void);
void UpdateVariables(void);
void Boom(float x, float y, bool cast_shrap, bool force);
void Shrap(float x, float y, unsigned char sh_type, unsigned int scnt);
long GetFreeSprite(bool force);
void SetUpWorld(void);
void DestroyWorld(void);
void RenderSprites(sprite objs[], unsigned int& objCnt);
void RenderSprites2(void);
void RenderTF();
bool LoadTexture(cTexture& MyTexture, const wchar_t* szFileName, bool force256);
bool LoadTextureAlpha(cTexture& MyTexture, const wchar_t* szFileName, bool force256);
bool LoadBitmapFromBMPFile(const wchar_t* szFileName, HBITMAP* phBitmap, HPALETTE* phPalette);
void RotateWorld(void);
void MeshLights();


world_type WLD;	// Contains 3D objects.
BYTE* video_buffer = NULL;	// Pointers for our screen keyboard_buffer, A B G R bytes.
BYTE* video_bufferB = NULL;	// tmp keyboard_buffer for the same purpose.
UCHAR keyboard_buffer[256];
int	video_buffer_row_size = 0;
Uint64 tmr_render;		// Tracks the number of ms to render one frame
Uint64 fps_Time = 0;		// Tracks time for FPS counter.

zp_size_t rendered_sprites = 0; // Statistics...
zp_size_t rendered_objects = 0;
zp_size_t total_faces = 0;
zp_size_t rendered_faces = 0;

// Variables relating to the Camera object (our fighter)
float an_X = 30, an_Y = 0, an_Z = 0;	// View Point Angle
float CamX = 0, CamY = 0, CamZ = 0;	// View Point Pos
float CamZoomY = -20;
zp_size_t CamOb = 0;

// Indexes in to sprite textures.
long ShotSprite = 0;
long ShrapSprite = 0;
long BoomSprite = 0;
long ShotSprite_len = 0;	// and animation lengths if applicable.
long ShrapSprite_len = 0;
long BoomSprite_len = 0;


const float FOV = 300;		// Sort of like field of view.

float scale = 5;
unsigned int TextureCnt = 0;

const unsigned int mines_cnt = 100;
cTexture Texture[250] = {};

const unsigned int SCREEN_WIDTHh = SCREEN_WIDTH / 2;
const unsigned int SCREEN_HEIGHTh = SCREEN_HEIGHT / 2;

//float iX,iY,iZ,oX,oY,oZ;	// intersection returns
const std::wstring PATH_ASSETS = L"assets\\";








sprite* sprites_back = 0;
unsigned int sprites_back_cnt = 0;
sprite* sprites_front = 0;
unsigned int sprites_front_cnt = 0;
sprite* sprite_tmp = 0;

long SelTxr = 0;
long chk_TextureShow = 0;

float ZBuffer[SCREEN_WIDTH * SCREEN_HEIGHT] = {};
float* pZBuffer = 0;
long ZBuffer_Offset = 0;
long ZBufferPage = 0;

int TmpScreen[SCREEN_WIDTH * SCREEN_HEIGHT * 4] = {};

object_type* camobj = 0;
object_type* curobj = 0;



cliped_polygon_type pclip;


double globalprint = 0.0f;
double debug1 = 0.0f;
double debug2 = 0.0f;
double debug3 = 0.0f;

// USING DECLARATION //////////////////////////////////////////////////////////////////////////////





#include <iostream>
#include <sstream>
void Output(const wchar_t* szFormat, ...)
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


void DeInit()
{
	DestroyWorld();
}

void Init()
{
	video_buffer_row_size = SCREEN_WIDTH * 4;

	SetUpWorld();
	fps_Time=SDL_GetTicks();// SDL_GetTicks();

	// Return to Windows
	//return true;

} 



// Game_Main () ///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// Main game loop /////////////////////////////////////////////////////////////////////////////////

char* Game_Main (unsigned char *lpSurface)
{
		
	tmr_render = SDL_GetTicks();

	// Store pointer to video-memory
	video_buffer = video_bufferB = (BYTE *)lpSurface;
	
	/////////////////////////////////////////////
	////// Begin Render /////////////////////////


	RotateWorld();

	// Clear ZBuffer if needed 
	if(ZBufferPage==100)
	{
		ZBufferPage = 1;
		memset(ZBuffer,1,sizeof(ZBuffer));
	} else ZBufferPage++;

	rendered_sprites = 0;
	rendered_objects = 0;
	total_faces = 0;
	rendered_faces = 0;

	RenderSprites(sprites_back, sprites_back_cnt);
	RenderTF();
	RenderSprites2();
	
	
  
	/////////////////////////////////////////////
	////// End Render /////////////////////////

	// Process keyboard input
	ProcessInput();

	
	// Maintain variables within limits
	UpdateVariables ();


	tmr_render = SDL_GetTicks() - tmr_render;
	// Return success
	return ( NULL );

}

void render_debug(SDL_Renderer* renderer)
{
	Uint64 tmr = SDL_GetTicks() - fps_Time;
	fps_Time = SDL_GetTicks();
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 200);

	std::string keyboard_buffer = std::format("Rendered Sprites {}/{} - Objects {}/{} - Faces {}/{}", rendered_sprites, sprites_back_cnt + sprites_front_cnt, rendered_objects, WLD.objcount, rendered_faces, total_faces);
	SDL_RenderDebugText(renderer, 0, 5, keyboard_buffer.c_str());

	keyboard_buffer = std::format("{} fps, render {} ms", 1000 / tmr, tmr_render);
	SDL_RenderDebugText(renderer, 0, 20, keyboard_buffer.c_str());
	
}



// https://wiki.libsdl.org/SDL3/SDL_Scancode

void handle_key_event_down(SDL_Scancode code)
{ 
	if(code >0 && code < 256) keyboard_buffer[code] = 0x01;
}
void handle_key_event_up(SDL_Scancode code)
{
	if (code > 0 && code < 256)	keyboard_buffer[code] = 0x00;
}

void ProcessInput ( void )
{
	#define KEYDOWN(name, key) (name[key] & 0x80) 

	if (keyboard_buffer[SDL_SCANCODE_Q]) {
		fps_Time = SDL_GetTicks();
	}

	if (keyboard_buffer[SDL_SCANCODE_W])	CamZoomY=0;
	if (keyboard_buffer[SDL_SCANCODE_Z])	CamZoomY+=10;
	if (keyboard_buffer[SDL_SCANCODE_X])	CamZoomY-=10;
	if (keyboard_buffer[SDL_SCANCODE_A])	CamZoomY+=1;
	if (keyboard_buffer[SDL_SCANCODE_S])	CamZoomY-=1;
	
	if (keyboard_buffer[SDL_SCANCODE_B])	an_X=-90;
	if (keyboard_buffer[SDL_SCANCODE_N])	an_X++;
	if (keyboard_buffer[SDL_SCANCODE_M])	an_X--;

	if (keyboard_buffer[SDL_SCANCODE_G])	an_Y++;
	if (keyboard_buffer[SDL_SCANCODE_H])	an_Y--;
	
	if (keyboard_buffer[SDL_SCANCODE_J])	an_Z++;
	if (keyboard_buffer[SDL_SCANCODE_K])	an_Z--;

	if (keyboard_buffer[SDL_SCANCODE_C])	Shrap(WLD.obj[CamOb].x,WLD.obj[CamOb].z,1,100);
	if (keyboard_buffer[SDL_SCANCODE_RIGHT])
	{
		WLD.obj[CamOb].turnpitch = 2;
		//WLD.obj[CamOb].an_Y+=6;
	}
	if (keyboard_buffer[SDL_SCANCODE_LEFT])
	{
		WLD.obj[CamOb].turnpitch = -2;
		//WLD.obj[CamOb].an_Y-=6;
	}
	if(WLD.obj[CamOb].an_Y>359) WLD.obj[CamOb].an_Y = WLD.obj[CamOb].an_Y - 360;
	if(WLD.obj[CamOb].an_Y<0) WLD.obj[CamOb].an_Y = 360 + WLD.obj[CamOb].an_Y;
	//if(WLD.obj[0].an_Z>359) WLD.obj[0].an_Z = WLD.obj[0].an_Z - 360;
	//if(WLD.obj[0].an_Z<0) WLD.obj[0].an_Z = 360 + WLD.obj[0].an_Z;

	
	if (keyboard_buffer[SDL_SCANCODE_SPACE])
	{
		WLD.obj[CamOb].shoot = true;
	}

	if (keyboard_buffer[SDL_SCANCODE_UP])
	{
		WLD.obj[CamOb].thrust = 1;
	}

	if (keyboard_buffer[SDL_SCANCODE_DOWN])
	{
		WLD.obj[CamOb].thrust = -1;
	}

	/*
	if ( keyboard_buffer[DIK_RALT])	input_scale *= 3;
	if ( keyboard_buffer[DIK_EQUALS])	cube.zPos   += ( 3 * input_scale );
	if ( keyboard_buffer[DIK_MINUS])	cube.zPos   -= ( 3 * input_scale );
	if ( keyboard_buffer[DIK_LBRACKET])	speed -= input_scale;
	if ( keyboard_buffer[DIK_RBRACKET])	speed += input_scale;
*/

}


void Boom(float x,float y,bool cast_shrap,bool force)
{
	
	float tx;
	float tz;
	for(zp_size_t tmp=0;tmp < sprites_front_cnt;tmp++)
	{

		if(sprites_front[tmp].visable)
		{
			tx = sprites_front[tmp].x - x;
			tz = sprites_front[tmp].z - y;
			if((sqrt((tx * tx) + (tz * tz)) < 5))
			{
				return;
			}
		}
	}

	long smObj = GetFreeSprite(force);
	if(smObj==-1) return;
	sprites_front[smObj].x = x;
	sprites_front[smObj].y = -(float)(rand()%30);
	sprites_front[smObj].z = y;
	sprites_front[smObj].vx = 0;
	sprites_front[smObj].vz = 0;
	sprites_front[smObj].vy = 0;
	sprites_front[smObj].liveTime = BoomSprite_len;
	sprites_front[smObj].visable = true;
	sprites_front[smObj].animation_start = BoomSprite;
	sprites_front[smObj].animation_len = BoomSprite_len;
	sprites_front[smObj].boTexture = &Texture[BoomSprite];
	sprites_front[smObj].alpha = 1;
	sprites_front[smObj].scale = true;

	if(cast_shrap) Shrap(x,y,0,2);
}

long GetFreeSprite (bool force)
{
	int smLiveTime = 10000;
	zp_size_t smObj = -1;
	for(zp_size_t tmp=0;tmp < sprites_front_cnt;tmp++)
	{
		if(sprites_front[tmp].visable == false)
		{
			smObj = tmp;
			break;
		}
		if(force && sprites_front[tmp].liveTime<smLiveTime && sprites_front[tmp].animation_start != ShotSprite)
		{
			smObj = tmp;
			smLiveTime = sprites_front[tmp].liveTime;
		}
	}

	return smObj;

}
void Shrap(float x, float y,unsigned char sh_type, unsigned int scnt)
{
	zp_size_t smObj;
	for(unsigned int t=1;t<scnt;t++)
	{
		smObj = GetFreeSprite(false);
		if(smObj==-1) return;
		//debug3 = sprites_front[tmp].visable;
		sprites_front[smObj].visable = true;
		sprites_front[smObj].x =  x;
		sprites_front[smObj].z =  y;
		sprites_front[smObj].y =  0;
		float ay = static_cast<float>(rand()%360);
		sprites_front[smObj].vz = (MyCos(ay)/10 * 50);
		sprites_front[smObj].vx = (MySin(ay)/10 * 50);
		sprites_front[smObj].liveTime = 190;
		if(sh_type==0)
		{
			sprites_front[smObj].vy = (MyCos(ay) * 20);
			sprites_front[smObj].vz/=2;
			sprites_front[smObj].vx/=2;

			sprites_front[smObj].animation_start = ShrapSprite;
			sprites_front[smObj].animation_len = ShrapSprite_len;
			sprites_front[smObj].boTexture = &Texture[ShrapSprite];
			sprites_front[smObj].alpha = 1;
		} else {
			sprites_front[smObj].vy = 0;
			sprites_front[smObj].animation_start = ShotSprite;
			sprites_front[smObj].animation_len = ShotSprite_len;
			sprites_front[smObj].boTexture = &Texture[ShotSprite];
			sprites_front[smObj].alpha = 1;
		}	
	}

}




void UpdateVariables ( void )
{
	float tx;
	float tz;
	object_type *curobj;


	// Ship is shooting so make a ranbow pompom fly out the front end..

	if(camobj->shoot)
	{
		camobj->shoot = false;
		float ay = camobj->an_Y;
		zp_size_t smObj = GetFreeSprite(true);
		sprite_tmp = &sprites_front[smObj];
		if(smObj==-1) return;
		//debug3 = sprites_front[tmp].visable;
		sprite_tmp->visable = true;
		sprite_tmp->x =  camobj->x;
		sprite_tmp->z =  camobj->z;
		sprite_tmp->y =  camobj->y;
		sprite_tmp->vz = (MyCos(ay)/10 * 50)+camobj->vz;
		sprite_tmp->vx = (MySin(ay)/10 * 50)-camobj->vx;
		sprite_tmp->vy = 0;
		sprite_tmp->liveTime = 190;
		sprite_tmp->animation_start = ShotSprite;
		sprite_tmp->animation_len = ShotSprite_len;
		sprite_tmp->boTexture = &Texture[ShotSprite];
		sprite_tmp->alpha = 1;
	}

	
	// Handle collisions between object<>CamOb, object<>object and object<>sprites.
	// Also to physics updates.
	// CamOb is the index of our ship, that the camera is atached to.

	for (zp_size_t ob = 0; ob < WLD.objcount; ob++)
	{
		curobj = &WLD.obj[ob];

		if (!curobj->scale) continue;	// Destroyed.

		// This just causes object to magically stay contained in a given area.
		// just like the good old days.
		BOOL moved = false;
		if (curobj->x < -500) {curobj->x = 500; moved = true;}
		if (curobj->x > 500) {curobj->x = -500; moved = true;}
		if (curobj->y < -500) { curobj->y = 500; moved = true; }
		if (curobj->y > 500) { curobj->y = -500; moved = true; }
		if (curobj->z < -500) { curobj->z = 500; moved = true; }
		if (curobj->z > 500) { curobj->z = -500; moved = true; }

		if (moved)
		{
			curobj->hidden = false;
			for (zp_size_t b = 0; b < curobj->polycount; b++)
			{
				curobj->polygon[b].visable = 0;
			}
		}

		if(ob!=CamOb)
		{	
			// Our method of astroid destruction is to gradually shrink it as it takes damage.
			// When it's small enough go boom.
			if (curobj->scale < 1 && curobj->scale != 0) {
				curobj->scale = 0;
				//curobj->y = -1000;
				//if(!curobj->hidden)
				Boom(curobj->x, curobj->z, true, true);
				Shrap(curobj->x, curobj->z, 1, 2);
				Shrap(curobj->x, curobj->z, 0, 2);
				continue;
			}

			// Each object has a volocity (vx, vy, vz), and angular velocity (vax, vay, vaz)
			curobj->an_X += curobj->vax;  FixAngle(curobj->an_X);
			curobj->an_Y += curobj->vay;  FixAngle(curobj->an_Y);
			curobj->an_Z += curobj->vaz;  FixAngle(curobj->an_Z);
			curobj->x += curobj->vx;
			curobj->y += curobj->vy;
			curobj->z += curobj->vz;
		}
		
		// Test curobj against all other objects/sprites for collisions this includes our ship.
		for(zp_size_t obb=ob+1;obb<WLD.objcount;obb++)
		{
			if (WLD.obj[obb].scale!=0) // && ob != CamOb
			{
				object_type* curobj2 = &WLD.obj[obb];
				tx = curobj2->x - curobj->x;
				tz = curobj2->z - curobj->z;
				float distance = sqrt((tx * tx) + (tz * tz));
				if(distance < ((curobj->bounds + curobj2->bounds)))
				{
					float ux = tx / distance;
					float uz = tz / distance;
					float rvx = curobj2->vx - curobj->vx;
					float rvz = curobj2->vz - curobj->vz;
					float vNormal = rvx * ux + rvz * uz;
					if (vNormal > 0) continue;
					float impulse = (1 * vNormal) / (curobj->mass + curobj2->mass);
					curobj->vx += impulse * curobj2->mass * ux;
					curobj->vz += impulse * curobj2->mass * uz;
					curobj2->vx -= impulse * curobj->mass * ux;
					curobj2->vz -= impulse * curobj->mass * uz;
					
					//Boom(curobj->x,curobj->z,false,false);
				}
			}
		}

		// does particle collision..
		if(ob!=CamOb)
		{
			for(zp_size_t a = 0;a < sprites_front_cnt;a++)
			{
				sprite_tmp = &sprites_front[a];
				if(sprite_tmp->visable == true && sprite_tmp->animation_start == ShotSprite)
				{
					tx = curobj->x - sprite_tmp->x;
					tz = curobj->z - sprite_tmp->z;
					if(sqrt((tx * tx) + (tz * tz)) < (curobj->bounds))
					{
						curobj->scale -= static_cast<float>(0.001);
						sprites_front[a].visable = false;
						if(sprite_tmp->animation_start == ShotSprite)
						{
							tx = sprite_tmp->x;
							tz = sprite_tmp->z;
							curobj->scale -=static_cast<float>(0.05);
							//Shrap(tx, tz, 0, 5);
							//if(!curobj->hidden)
							Boom(tx, tz, true, true);
						}
							
							
					}

				}
			}
				
		}

	}

	if(camobj->thrust!=0)
	{
		float ay = round(camobj->an_Y);
		camobj->vz+=MyCos(ay)/10 * camobj->thrust;
		camobj->vx-=MySin(ay)/10 * camobj->thrust;
	}

	if(camobj->vz > 10) camobj->vz = 10;
	if(camobj->vx > 10) camobj->vx = 10;
	if(camobj->vz < -10) camobj->vz = -10;
	if(camobj->vx < -10) camobj->vx = -10;

	camobj->z += camobj->vz;
	camobj->x -= camobj->vx;

	if(camobj->vz < 0) camobj->vz+=static_cast<float>(0.01);
	if(camobj->vz > 0) camobj->vz-=static_cast<float>(0.01);
	if(camobj->vx < 0) camobj->vx+=static_cast<float>(0.01);
	if(camobj->vx > 0) camobj->vx-=static_cast<float>(0.01);

	// Highlight 
	Texture[1].faceB = 255 * abs((int)camobj->thrust);
	if (Texture[0].faceB > 255) Texture[0].faceB = 255;
	Texture[5].faceR = Texture[0].faceR;
	Texture[5].faceG = 0;
	
	camobj->thrust = 0;

	if(camobj->turnpitch)
	{
		camobj->an_Z+=camobj->turnpitch;
		if(camobj->an_Z>45) camobj->an_Z=45;
		if(camobj->an_Z<-45) camobj->an_Z=-45;
		camobj->turnpitch = 0;
	} else {
		if(camobj->an_Z!=0) camobj->an_Z/=1.05f;
	}
	
	camobj->an_Y+=camobj->an_Z/5;


	CamY = camobj->y -(190 + CamZoomY);// -((sqrt(camobj->vx * camobj->vx + camobj->vz * camobj->vz) * 10) + 200 + CamZoomY);
	CamX = camobj->x;
	CamZ = camobj->z;

	
	CamZ = camobj->z - (-CamY * MyCos(an_X));
	CamY = camobj->y - (CamY * MySin(an_X));

	
	for(zp_size_t a = 0;a < sprites_front_cnt;a++)
	{
		sprite_tmp = &sprites_front[a];
		if(sprite_tmp->visable == false) continue;
		
		
		if(sprite_tmp->animation_start != BoomSprite) sprite_tmp->alpha-=static_cast<float>(0.005);
		debug3++;


		//sprite_tmp->vx = (curobj->x-sprite_tmp->x)/100;
		//sprite_tmp->vz = (curobj->z-sprite_tmp->z)/100;


		if(sprite_tmp->animation_len > 0)
		{
			//debug3 = 
			if(sprite_tmp->liveTime > sprite_tmp->animation_len)
			{
				sprite_tmp->boTexture = &Texture[sprite_tmp->animation_start + rand()%sprite_tmp->animation_len];
			} else {
				sprite_tmp->boTexture = &Texture[sprite_tmp->animation_start + (sprite_tmp->animation_len - sprite_tmp->liveTime)];
			}
		}
		sprite_tmp->x += sprite_tmp->vx;
		sprite_tmp->y += sprite_tmp->vy;
		sprite_tmp->z += sprite_tmp->vz;

		sprite_tmp->liveTime--;
		if (sprite_tmp->liveTime == 0)
		{
			sprite_tmp->visable = false;
			sprite_tmp->liveTime = 0;
			continue;
		}
	}
	
}



inline void RenderSprites(sprite objs[], unsigned int &objCnt)
{

	//return;
	BYTE *MyTextureP;
	float qqqc = 0;
	int rundistance;
	int runstart;
	int runtostart;

	int Vrundistance;
	
	int videoXOff;
	int videoXOffb;
	int videoYOff;
	//unsigned int sprite_width;
	//unsigned int sprite_height;

	for(zp_size_t tmp=0;tmp<objCnt;tmp++)
	{
		sprite_tmp = &objs[tmp];
		
		if(!sprite_tmp->visable) continue;

		if(sprite_tmp->p)
		{
			sprite_tmp->lz = 1/sprite_tmp->lz;
			sprite_tmp->lx = ((sprite_tmp->lx) * sprite_tmp->lz * FOV) + SCREEN_WIDTHh;
			sprite_tmp->ly = ((sprite_tmp->ly) * sprite_tmp->lz * FOV) + SCREEN_HEIGHTh;
		} else {
			sprite_tmp->lx = sprite_tmp->x + SCREEN_WIDTHh;
			sprite_tmp->ly = sprite_tmp->y + SCREEN_HEIGHTh;
		}
		if(sprite_tmp->lz < 0) continue;
		if(sprite_tmp->lx < 0 || sprite_tmp->lx>SCREEN_WIDTH || sprite_tmp->ly < 0 || sprite_tmp->ly>SCREEN_HEIGHT) continue;
		//float tsc;
		//if(sprite_tmp->ly > SCREEN_HEIGHT && sprite_tmp->ly < 0 && sprite_tmp->lx > SCREEN_WIDTH && sprite_tmp->lx < 0) continue;


		//continue;
		// Get pointer to this background objects texture
		cTexture *ThisTexture = sprite_tmp->boTexture;
		MyTextureP = ThisTexture->bmpBuffer;

		// Get video offset of this sprite
		videoXOff = static_cast<int>(sprite_tmp->lx - ThisTexture->bmWidth / 2);
		videoYOff = static_cast<int>(sprite_tmp->ly - ThisTexture->bmHeight / 2);

		rundistance = ThisTexture->bmWidth + videoXOff; //  + CamX


		if(videoXOff < 0)					// Clip to left of screen
		{
			if(rundistance<1) continue;				// off screen left, next...
			runstart = -videoXOff*4;
			videoXOff = 0;			
			runtostart = 0;

		} else if(rundistance > SCREEN_WIDTH ) {	// Clip right

			if(videoXOff > SCREEN_WIDTH) continue;
			runstart = 0;
			rundistance = SCREEN_WIDTH - videoXOff;
			runtostart = (ThisTexture->bmWidth - rundistance)*4;

		} else {

			rundistance = ThisTexture->bmWidth;
			runstart = 0;
			runtostart = 0;
		}


		if(videoYOff < 0)
		{

			if(videoYOff + ThisTexture->bmHeight < 1) continue;
			MyTextureP+= (-videoYOff) * ThisTexture->bmWidth * 4;
			Vrundistance = videoYOff + ThisTexture->bmHeight;
			videoYOff = 0;

		} else if(ThisTexture->bmHeight + videoYOff > SCREEN_HEIGHT)
		{

			if(videoYOff > SCREEN_HEIGHT) continue;
			Vrundistance = SCREEN_HEIGHT - videoYOff;

		} else {

			Vrundistance = ThisTexture->bmHeight;	

		}

		videoXOffb = videoXOff;
		videoXOff*=4;

		if(sprite_tmp->zbuffer)
		{
			video_buffer = &video_bufferB[(0 + videoYOff) * video_buffer_row_size + videoXOff];
			pZBuffer = &ZBuffer[(0 + videoYOff) * SCREEN_WIDTH + videoXOffb];	
			for(int y = 0; y < Vrundistance; y++)
			{
				//video_buffer = &video_bufferB[(y + videoYOff) * video_buffer_row_size + videoXOff];
				//pZBuffer = &ZBuffer[(y + videoYOff) * SCREEN_WIDTH + videoXOffb];	
				MyTextureP+=runstart;

				for(int x = 0; x < rundistance; x++)
				{ 
					if(MyTextureP[0] + MyTextureP[1] + MyTextureP[2])
					{
						if((qqqc = sprite_tmp->lz+ZBufferPage) > pZBuffer[0])
						{
							pZBuffer[0] = qqqc;	
							video_buffer[1] = MyTextureP[0];
							video_buffer[2] = MyTextureP[1];
							video_buffer[3] = MyTextureP[2];

						}
					}

					video_buffer+=4;
					MyTextureP+=4;
					pZBuffer++;
				}

				MyTextureP+=runtostart;

			}
		} else {
			for(int y = 0; y<Vrundistance ;y++)
			{
				video_buffer = video_bufferB + (y+ videoYOff) * video_buffer_row_size + videoXOff;
				MyTextureP+=runstart;

				for(int x = 0; x<rundistance;x++)
				{
					video_buffer[1] = MyTextureP[0];
					video_buffer[2] = MyTextureP[1];
					video_buffer[3] = MyTextureP[2];

					video_buffer+=4;
					MyTextureP+=4;
				}

				MyTextureP+=runtostart;
			}
		}

		rendered_sprites += 1;

	}


}



inline void RenderSprites2(void)
{
	//debug1 = SDL_GetTicks();
	//debug1 = SDL_GetTicks();


	sprite *obj;
	BYTE *MyTexturePP;
	BYTE *MyTextureP;

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
	cTexture *ThisTexture;

	for(zp_size_t tmp=0;tmp<sprites_front_cnt;tmp++)
	{
	
		obj = &sprites_front[tmp];

		if(!obj->visable) continue;
		obj->lz = 1/obj->lz;
		obj->lx = ((obj->lx) * obj->lz * FOV) + SCREEN_WIDTHh;
		obj->ly = ((obj->ly) * obj->lz * FOV) + SCREEN_HEIGHTh;
		if(obj->lx < 0 || obj->lx>SCREEN_WIDTH || obj->ly < 0 || obj->ly>SCREEN_HEIGHT) continue;
		if(obj->lz<0)continue;

		// Get pointer to this background objects texture
		ThisTexture = obj->boTexture;
		//\scale = 2;
		// Get video offset of this sprite
		offset_x = static_cast<int>(obj->lx - (scale * ThisTexture->bmWidth / 2));
		offset_y = static_cast<int>(obj->ly - (scale * ThisTexture->bmHeight / 2));
		scale = obj->lz * 200 * obj->oscale;
		//debug1 = obj->lz;

		sprite_w = ThisTexture->bmWidth;
		sprite_h = ThisTexture->bmHeight;
		sprite_sw = static_cast<int>(sprite_w * scale);
		sprite_sh = static_cast<int>(sprite_h * scale);
		sprite_st = 0;
		sprite_sl = 0;


		// Video Set up
		//int video_sx = offset_x * 3; 
		video_rx = 4 * int(SCREEN_WIDTH - sprite_sw);
		// Sprite Set up
		scaleStep= 1 / scale;
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
			sprite_sh = SCREEN_HEIGHT - offset_y;
		}

		if(offset_x<0)
		{
			if(offset_x + sprite_sw < 1) continue;
			sprite_sl = -offset_x;
			scaleCnt_st = sprite_sl * scaleStep;
			video_rx = 4 * int(SCREEN_WIDTH - sprite_sw - offset_x);
			offset_x = 0;
		} 
		if (offset_x + sprite_sw > SCREEN_WIDTH)
		{
			if(offset_x >= SCREEN_WIDTH) continue;
			sprite_sw = SCREEN_WIDTH - offset_x;
			video_rx = 4 * int(SCREEN_WIDTH - sprite_sw);
		}

		//float alpha = obj->alpha;
		objalpha = obj->alpha;
		objalphaInv = 1 - obj->alpha;

		MyTextureP = ThisTexture->bmpBuffer;
		MyTexturePP = MyTextureP;

		scaleCnt = 0;
		qqqc = 0;
		video_rxz = video_rx / 4;
	

		video_buffer = &video_bufferB[offset_y * video_buffer_row_size + (offset_x * 4)];
		pZBuffer = &ZBuffer[offset_y * SCREEN_WIDTH + offset_x];
	
		textureVpos = sprite_st / scale;
		textureVstep = (sprite_sh / scale - textureVpos) / (sprite_sh - sprite_st);;
		sprite_w = sprite_w << 2;

		for(y = sprite_st; y < sprite_sh; y++)
		{
			scaleCnt = scaleCnt_st;
			MyTexturePP = &ThisTexture->bmpBuffer[int(textureVpos) * sprite_w];

			if(objalpha!=1)
			{

				for(x = sprite_sl; x < sprite_sw;x++)
				{
					if((qqqc = obj->lz+ZBufferPage) > pZBuffer[0])
					{
						MyTextureP = &MyTexturePP[int(scaleCnt) * 4 ];
						//Output(L"x=%i y=%i p=%p\r\n", x, y, MyTextureP);
						//Output(L"%i %i %i\r\n", MyTextureP[0] , MyTextureP[1] , MyTextureP[2]);
						if(MyTextureP[0]+MyTextureP[1]+MyTextureP[2])
						{

							pZBuffer[0] = qqqc;
							video_buffer[1] = MyTextureP[0];// *objalpha + video_buffer[1] * objalphaInv;
							video_buffer[2] = MyTextureP[1];// *objalpha + video_buffer[2] * objalphaInv;
							video_buffer[3] = MyTextureP[2];// *objalpha + video_buffer[3] * objalphaInv;
						}
					}

					video_buffer+=4;
					scaleCnt+=scaleStep;
					pZBuffer++;
				}
			} else {
				if(ThisTexture->hasAlpha)
				{

					for(x = sprite_sl; x < sprite_sw;x++)
					{
						if((qqqc = obj->lz+ZBufferPage) > pZBuffer[0])
						//{
							//pZBuffer[0] = qqqc;

							MyTextureP = &MyTexturePP[int(scaleCnt) * 4];
							if(MyTextureP[0]+MyTextureP[1]+MyTextureP[2])
							{
							// Video Cast
								alpha = static_cast<float>(MyTextureP[3])*0.00392f;
								alphaInv = 1 - alpha;
								video_buffer[1] = static_cast<BYTE>((video_buffer[1] * alphaInv) + (MyTextureP[0] * alpha));
								video_buffer[2] = static_cast<BYTE>((video_buffer[2] * alphaInv) + (MyTextureP[1] * alpha));
								video_buffer[3] = static_cast<BYTE>((video_buffer[3] * alphaInv) + (MyTextureP[2] * alpha));
							}
						//}
						video_buffer+=4;
						scaleCnt+=scaleStep;
						pZBuffer++;
					}

				} else {

					for(x = sprite_sl; x < sprite_sw;x++)
					{
						if((qqqc = obj->lz+ZBufferPage) > pZBuffer[0])
						{
							MyTextureP = &MyTexturePP[int(scaleCnt) * 2];
							if(MyTextureP[0]+MyTextureP[1]+MyTextureP[2])
							{
							pZBuffer[0] = qqqc;
							video_buffer[1] = MyTextureP[0];
							video_buffer[2] = MyTextureP[1];
							video_buffer[3] = MyTextureP[2];
							}
						}
						video_buffer+=4;
						scaleCnt+=scaleStep;
						pZBuffer++;
					}

				}


			}

			textureVpos += textureVstep;
			video_buffer += video_rx;
			pZBuffer += video_rxz;
		}

		rendered_sprites += 1;

	}
//debug1 = SDL_GetTicks()-debug1;
//debug1 = SDL_GetTicks() - debug1;
//if(debug1) debug1 = debug1;
}



// DATA STRUCTURES ////////////////////////////////////////////////////////////////////////////////

// FUNCTION PROTOTYPES ////////////////////////////////////////////////////////////////////////////
// GLOBALS ////////////////////////////////////////////////////////////////////////////////////////

// FUNCTIONS //////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rotate the world poly set and lights about CamX,CamY,CamZ
//	return new world as Obt & LSt
//	perform basic culling.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void RotateWorld(void)
{	
	zp_size_t a=0,b=0;
	//object_type *curobj;
	vertex_type *curvertex;

	/*
	for(a=0;a<WLD.objcount;a++)
	{
		curobj = &WLD.obj[a];
		curobj->wx = curobj->x;
		curobj->wy = curobj->y;
		curobj->wz = curobj->z;

		for(b=0;b<curobj->vertcount;b++)
		{
			curvertex = &curobj->vertex[b];
			// Copy vertex
			curvertex->wx = curvertex->lx * curobj->scale;
			curvertex->wy = curvertex->ly * curobj->scale;
			curvertex->wz = curvertex->lz * curobj->scale;
			debug1 = curobj->scale;
		}

	}
	*/

	float globalMatrix[4][4];
	Matrix_Ident(globalMatrix);
	RotatePntExB(globalMatrix,-an_X, an_Y, an_Z);

	float obMatrix[4][4];

	float zover;

	for(a=0;a<WLD.objcount;a++)
	{
		
		curobj = &WLD.obj[a];

		// Copy vert .. obj center
		curobj->wx = curobj->x;
		curobj->wy = curobj->y;
		curobj->wz = curobj->z;

		Matrix_Ident(obMatrix);
		if (a == 101) {
			int a = 12;
		}
		RotatePntExB(obMatrix,curobj->an_X, curobj->an_Y, curobj->an_Z);
		Translate(obMatrix,curobj->x,curobj->y,curobj->z);

		for(b=0;b<curobj->vertcount;b++)
		{
			curvertex = &curobj->vertex[b];

			// Copy & Scale vert
			curvertex->wx = curvertex->lx * curobj->scale;
			curvertex->wy = curvertex->ly * curobj->scale;
			curvertex->wz = curvertex->lz * curobj->scale;

			// Rotate by object
			Transform(obMatrix,curvertex->wx, curvertex->wy, curvertex->wz);

			// Rotate & translate to camera.
			curvertex->wx = curvertex->wx - CamX;
			curvertex->wy = curvertex->wy - CamY;
			curvertex->wz = curvertex->wz - CamZ;

			Transform(globalMatrix,curvertex->wx, curvertex->wy, curvertex->wz);
		}

		// Translate & Rotate & Project object center into screen space.
		curobj->wx = curobj->wx - CamX;
		curobj->wy = curobj->wy - CamY;
		curobj->wz = curobj->wz - CamZ;
		Transform(globalMatrix,curobj->wx,curobj->wy,curobj->wz);
		zover = (1/curobj->wz) * FOV;
		curobj->wx = (curobj->wx * zover) + SCREEN_WIDTHh;
		curobj->wy = (curobj->wy * zover) + SCREEN_HEIGHTh;

	}
	
	// Transform Sprites./////////////////////////////////////

	sprite *sprite_tmp;

	for(a = 0;a < sprites_back_cnt;a++)
	{
		sprite_tmp = &sprites_back[a];
		if(sprite_tmp->distance_infinity)
		{
			sprite_tmp->lx = sprite_tmp->x;
			sprite_tmp->ly = sprite_tmp->y;
			sprite_tmp->lz = sprite_tmp->z;
		} else {
			sprite_tmp->lx = sprite_tmp->x - CamX;
			sprite_tmp->ly = sprite_tmp->y - CamY;
			sprite_tmp->lz = sprite_tmp->z - CamZ;
		}

		Transform(globalMatrix,sprite_tmp->lx, sprite_tmp->ly, sprite_tmp->lz);
	}
	for(a = 0;a < sprites_front_cnt;a++)
	{
		sprite_tmp = &sprites_front[a];
		sprite_tmp->lx = sprite_tmp->x - CamX;
		sprite_tmp->ly = sprite_tmp->y - CamY;
		sprite_tmp->lz = sprite_tmp->z - CamZ;
		Transform(globalMatrix,sprite_tmp->lx, sprite_tmp->ly, sprite_tmp->lz);
	}

	// Cull some of the 3d mesh's
	
	int cntobs=0;
	float tsc;
	for(a=0;a<WLD.objcount;a++)
	{

		curobj = &WLD.obj[a];
		tsc = curobj->scale * 50;
		if(curobj->wy - tsc < SCREEN_HEIGHT && curobj->wy + tsc  > 0 && curobj->wx  - tsc < SCREEN_WIDTH && curobj->wx + tsc > 0)
		{
			//debug3++;

			curobj->hidden = false;
			for(b=0;b<curobj->polycount;b++)
			{
				if(curobj->polygon[b].visable==0)
				{
					CalcNorm(curobj->polygon[b]);
					if(curobj->polygon[b].D<0) curobj->polygon[b].visable=int(10 * -curobj->polygon[b].D);
				} else curobj->polygon[b].visable--;

			}
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

inline void MeshLights()
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

	// number of objects in scene
	WLD.objcount=xloader.getObjectCount();
	// Alocate memory for objects.
	WLD.obj = new object_type[WLD.objcount];

	long vertCnt=0;
	long faceCnt=0;

	for (zp_size_t i=0; i<xloader.getObjectCount(); i++)
	{
		Mesh* curmesh = xloader.getObject(i);
		zp_size_t attrOffset = xloader.getAttrOffset(i);
		object_type *curobj = &WLD.obj[i];						// assogn pointer CUROBJ to current object

		curobj->thrust = 0;
		curobj->vx = 0;
		curobj->vy = 0;//;((rand()%10)-20)/10;
		curobj->vz = 0;
		curobj->an_X = 0;
		curobj->an_Y = 0;
		curobj->an_Z = 0;
		curobj->x = 0; // xloader.getObject(i)->x;
		curobj->y = 0; //-xloader.getObject(i).z;
		curobj->z = 0; //xloader.getObject(i).y;
		

		//curobj->vertcount = xloader.getObject(i).GetVertexCount();	// Number of verts in this object?
		curobj->vertcount = curmesh->mnVerts;		// Number of verts in this object?
		curobj->vertex = new vertex_type[curobj->vertcount];	// Allocate memory for vertex array;

		for(zp_size_t v=0; v<curobj->vertcount; v++)
		{
			vertex_type *curvert=&curobj->vertex[v];
			curvert->lx = curmesh->mPositions[v].x;
			curvert->ly = -curmesh->mPositions[v].z;	// was - .z
			curvert->lz = curmesh->mPositions[v].y; // was .y
			vertCnt++;
		}

		curobj->polycount = curmesh->mnFaces;	// Set number of polygons in object
		
		curobj->polygon = new polygon_type[curobj->polycount];	// Assign memory for polygo array


		for(zp_size_t p=0; p<curobj->polycount; p++)
		{
			polygon_type *curpoly = &curobj->polygon[p];

			// ********** NOT needed now since vertex count is always = 3
			//curpoly->vertcount = 3;
			// Assign memory for vertex array
			//curpoly->vertex = new vertex_type*[curpoly->vertcount];
			//curpoly->uv = new uv_type[curpoly->vertcount];
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

	}

	
	TextureCnt = xloader.getTextureCount();
	int tmpmapcnt=0;
	for(zp_size_t i=0; i<TextureCnt;i++)
	{
		bool loadmap = true;
		Mesh::Material* curtexture = xloader.getTexture(i);

		BYTE faceR = 0, faceG = 0, faceB = 0;
		Texture[i].faceR = static_cast<BYTE>(curtexture->diffuseColor.x * 255);
		Texture[i].faceG = static_cast<BYTE>(curtexture->diffuseColor.y * 255);
		Texture[i].faceB = static_cast<BYTE>(curtexture->diffuseColor.z * 255);
		
		if(curtexture->texture.length() > 0)
		{

			for(zp_size_t p=0; p<i; p++)
			{
				if(p!=i)
				{
					if(xloader.getTexture(p)->texture.compare(curtexture->texture)==0)
					//if(strcmp(xloader.GetTexture(i).bmFileName, Texture[p].filename))
					{
						for (zp_size_t ob = 0; ob < WLD.objcount; ob++)
						{
							for (zp_size_t v = 0; v < WLD.obj[ob].polycount; v++)
								if (WLD.obj[ob].polygon[v].SurfaceTexture == i)
								{
									WLD.obj[ob].polygon[v].SurfaceTexture = p;
								}
						}
						loadmap = false;
						break;
						Texture[i].bmpBuffer = Texture[p].bmpBuffer;
					}
				}
			}

			if(loadmap)
			{
				LoadTexture(Texture[i], curtexture->texture.c_str(), false);
				tmpmapcnt++;
			}
		}

	}
	

	xloader.clear();
}



void SetUpWorld(void)
{

	InitMath();
	
	TextureCnt=0;
	MeshLights();
	
	
	LoadTexture(Texture[TextureCnt++], L"star2.bmp",false);
	LoadTexture(Texture[TextureCnt++], L"earth.bmp",false);
	LoadTextureAlpha(Texture[TextureCnt-1], L"earth_alpha.bmp",false);
	
	sprites_back_cnt = 500;
	sprites_back = new sprite[sprites_back_cnt];
	
	float an = 0;
	float anb = 0;
	for(zp_size_t tmp=0;tmp < sprites_back_cnt;tmp++)
	{
		an = static_cast<float>(rand()%359);
		anb= static_cast<float>(rand()%359);
		Output(L"COS %f", MyCos(an));
		sprites_back[tmp].x =  MyCos(anb) * MyCos(an) * 20000;
		sprites_back[tmp].z =  MyCos(anb) * MySin(an) * 20000;
		sprites_back[tmp].y =  MySin(anb) * 20000;
		sprites_back[tmp].vx = 0;
		sprites_back[tmp].vy = 0;
		sprites_back[tmp].vz = 0;
		sprites_back[tmp].p = true;
		sprites_back[tmp].boTexture = &Texture[TextureCnt-2];
		sprites_back[tmp].visable = true;
		sprites_back[tmp].distance_infinity = true;
		sprites_back[tmp].zbuffer = false;
		sprites_back[tmp].scale = false;
		sprites_back[tmp].alpha = 1;
	}
	sprites_back[sprites_back_cnt-2].x =  20000;
	sprites_back[sprites_back_cnt-2].z =  10000;
	sprites_back[sprites_back_cnt-2].y =  20000;
	sprites_back[sprites_back_cnt-2].p = true;
	sprites_back[sprites_back_cnt-2].boTexture = &Texture[TextureCnt-1];
	sprites_back[sprites_back_cnt-2].visable = true;
	sprites_back[sprites_back_cnt-2].distance_infinity = false;
	sprites_back[sprites_back_cnt-2].zbuffer = false;
	sprites_back[sprites_back_cnt-2].scale = false;
	sprites_back[sprites_back_cnt-2].alpha = 1;

	sprites_front_cnt=5000;
	sprites_front = new sprite[sprites_front_cnt];	

	
	LoadTexture(Texture[TextureCnt++], L"shot_00.bmp",false);
	LoadTexture(Texture[TextureCnt++], L"shot_01.bmp",false);
	LoadTexture(Texture[TextureCnt++], L"shot_02.bmp",false);
	
	cTexture* t = &Texture[TextureCnt - 1];
	BYTE* b = t->bmpBuffer;

	for (int a = 0; a < 9; a++)
	{
		Output(L"%i %i %i %i\r\n", b[0], b[1], b[2], b[3]);
		b += 4;
	}

	for(zp_size_t tmp=0;tmp < sprites_front_cnt;tmp++)
	{
		sprites_front[tmp].x =  0;
		sprites_front[tmp].z = static_cast<float>(tmp);
		sprites_front[tmp].y =  0;
		sprites_front[tmp].vx = 0;
		sprites_front[tmp].vz = 0;
		sprites_front[tmp].vy = 0;
		sprites_front[tmp].p = true;
		sprites_front[tmp].liveTime = 0;
		sprites_front[tmp].visable = false;
		sprites_front[tmp].distance_infinity = false;
		sprites_front[tmp].zbuffer = true;
		sprites_front[tmp].scale = true;
		sprites_front[tmp].oscale = 0.3f;
		sprites_front[tmp].animation_len = 1;
		//sprites_front[tmp].animation_len = TextureCnt-1;
		sprites_front[tmp].boTexture = &Texture[TextureCnt-1];
		sprites_front[tmp].alpha = 0.5;
	}
	ShotSprite = TextureCnt-3;
	ShotSprite_len = 3;


	
	for(unsigned int tmp=0 ; tmp<62 ; tmp+=2)
	{
		std::wstring tbuffer = std::format(L"explosion\\explosion{:04}.bmp", tmp);
		LoadTexture(Texture[TextureCnt++],tbuffer.c_str(), false);
		tbuffer = std::format(L"explosion\\explosion_Alpha{:04}.bmp", tmp);
		LoadTextureAlpha(Texture[TextureCnt-1], tbuffer.c_str(), false);

	}
	LoadTexture(Texture[TextureCnt++], L"shrap.bmp", false);
	ShrapSprite = TextureCnt - 1;
	ShrapSprite_len = 1;

	// Set up forground sprites


	BoomSprite = TextureCnt-32;
	BoomSprite_len = 32;

	// Set up asteroids.
	int ststart = 1;
	//int an; 
	for(zp_size_t tmp=ststart;tmp<mines_cnt - ststart;tmp++)
	{
		//WLD.obj[tmp].x = rand()%1500 - 750;
		//WLD.obj[tmp].z = rand()%750-100;
		WLD.obj[tmp].x = (float)(rand()%1000 - 500);
		WLD.obj[tmp].z = (float)(rand()%1000 - 500);
		WLD.obj[tmp].y = 0; // (float)(rand() % 8 - 4);

		an = static_cast<float>(rand()%360-180);
		WLD.obj[tmp].vax = float(((rand()%10)-5))/5;//((MyCos(an) * 10)-5)/4;
		WLD.obj[tmp].vay = 0;
		WLD.obj[tmp].vaz = float(((rand()%10)-5))/5;//((MyCos(an) * 10)-5)/4;
		WLD.obj[tmp].vax = float(rand() % 10) / 10;
		WLD.obj[tmp].vaz = float(rand() % 10) / 10;
		an = static_cast<float>(rand()%359);
		anb = static_cast<float>(rand()%10);
		WLD.obj[tmp].vx =  (MyCos(an)/10 * anb);
		WLD.obj[tmp].vy =  0; //(rand()%2 - 1);
		WLD.obj[tmp].vz =  (MySin(an)/10 * anb);

		WLD.obj[tmp].scale = float(rand()%5+1);
		WLD.obj[tmp].mass = WLD.obj[tmp].scale;
		WLD.obj[tmp].bounds = WLD.obj[tmp].mass * 5;
	}
	
	WLD.obj[ststart].x = -100;
	WLD.obj[ststart].y = -100;
	WLD.obj[ststart].z = 0;
	WLD.obj[ststart].vax = 1;
	WLD.obj[ststart].vay = 1;
	WLD.obj[ststart].vaz = 1;
	WLD.obj[ststart].vx = 0;
	WLD.obj[ststart].vy = 0;
	WLD.obj[ststart].vz = 0;


	CamX = 0; CamY = -100; CamZ = 0;
	CamOb = 0;
	camobj = &WLD.obj[CamOb];
	camobj->x = 0;
	camobj->y = 0;
	camobj->z = 0;
	camobj->scale = 1;
	camobj->mass = 5;
	camobj->bounds = 5;
	CamZoomY = -150;
	an_X = -50;

	RotateWorld();
}


void DestroyWorld(void)
{
	delete [] sprites_back;
	delete [] sprites_front;

	for(zp_size_t a=0;a<TextureCnt;a++)
	{
		delete [] Texture[a].bmpBuffer;
	}
	
	for(zp_size_t a=0;a < WLD.objcount;a++)
	{
		delete [] WLD.obj[a].polygon;
		delete [] WLD.obj[a].vertex;
	
	}
	delete [] WLD.obj;
}





unsigned int polyx[SCREEN_HEIGHT][2];
float polyz[SCREEN_HEIGHT][2];
float polyuv[SCREEN_HEIGHT][4];




void zclip(polygon_type *polygon,cliped_polygon_type *clip)
{
	float t;
	clip_type *pcv=clip->vertex;

	int cp=0;
	float zmin=2;	// don't go below zero.

	int v1=2;
	
	vertex_type *pv1;
	vertex_type *pv2;
	uv_type *puv1;
	uv_type *puv2;

	for(int v2=0; v2<3; v2++)
	{
		pv1=polygon->vertex[v1];
		pv2=polygon->vertex[v2];

		puv1=&polygon->uv[v1];
		puv2=&polygon->uv[v2];

		if((pv1->wz >= zmin) && (pv2->wz >= zmin))
		{
			
			pcv[cp].x = pv2->wx;
			pcv[cp].y = pv2->wy;
			pcv[cp].u = puv2->u;
			pcv[cp].v = puv2->v;
			pcv[cp++].z = pv2->wz;
		}
		if((pv1->wz >= zmin) && (pv2->wz < zmin))
		{
			t=(zmin - pv1->wz) / (pv2->wz - pv1->wz);
			pcv[cp].x = pv1->wx + (pv2->wx - pv1->wx) * t;
			pcv[cp].y = pv1->wy + (pv2->wy - pv1->wy) * t;
			pcv[cp].u = puv1->u  + (puv2->u  - puv1->u) * t;
			pcv[cp].v = puv1->v  + (puv2->v  - puv1->v) * t;
			pcv[cp++].z = zmin;

		}
		if((pv1->wz < zmin) && (pv2->wz >= zmin))
		{
			t=(zmin - pv1->wz) / (pv2->wz - pv1->wz);
			pcv[cp].x = pv1->wx + (pv2->wx - pv1->wx) * t;
			pcv[cp].y = pv1->wy + (pv2->wy - pv1->wy) * t;
			pcv[cp].u = puv1->u + (puv2->u - puv1->u) * t;
			pcv[cp].v = puv1->v + (puv2->v - puv1->v) * t;
			pcv[cp++].z = zmin;
			pcv[cp].x = pv2->wx;
			pcv[cp].y = pv2->wy;
			pcv[cp].u = puv2->u;
			pcv[cp].v = puv2->v;
			pcv[cp++].z = pv2->wz;
		}
		v1 = v2;
	}

	clip->vertcount = cp;
}




void tclip(cliped_polygon_type *clip)
{
	clip_type *pcv=clip->vertex;
	clip_type *pv1;
	clip_type *pv2;

	float t;
	float ymin=1;
	float ymax=SCREEN_HEIGHT-1;
	float xmin=1;
	float xmax=SCREEN_WIDTH-1;

	// Clip to screen top at ymin
	int cp=0;
	int v1=clip->vertcount-1;

	for(zp_size_t v2=0; v2 < clip->vertcount; v2++)
	{
		pv1=&clip->vertex[v1];
		pv2=&clip->vertex[v2];
		if((pv1->y1 >= ymin) && (pv2->y1 >= ymin))
		{
			pcv[cp].z = pv2->z1;
			pcv[cp].x = pv2->x1;
			pcv[cp].u = pv2->u1;
			pcv[cp].v = pv2->v1;
			pcv[cp++].y = pv2->y1;
		}
		if((pv1->y1 >= ymin) && (pv2->y1 < ymin))
		{
			t=(ymin - pv1->y1) / (pv2->y1 - pv1->y1);
			pcv[cp].x = pv1->x1 + (pv2->x1 - pv1->x1) * t;
			pcv[cp].z = pv1->z1 + (pv2->z1 - pv1->z1) * t;
			pcv[cp].u = pv1->u1 + (pv2->u1 - pv1->u1) * t;
			pcv[cp].v = pv1->v1 + (pv2->v1 - pv1->v1) * t;
			pcv[cp++].y = ymin;
		}
		if((pv1->y1 < ymin) && (pv2->y1 >= ymin))
		{
			t=(ymin - pv1->y1) / (pv2->y1 - pv1->y1);
			pcv[cp].x = pv1->x1 + (pv2->x1 - pv1->x1) * t;
			pcv[cp].z = pv1->z1 + (pv2->z1 - pv1->z1) * t;
			pcv[cp].u = pv1->u1 + (pv2->u1 - pv1->u1) * t;
			pcv[cp].v = pv1->v1 + (pv2->v1 - pv1->v1) * t;
			pcv[cp++].y = ymin;
			pcv[cp].x = pv2->x1;
			pcv[cp].z = pv2->z1;
			pcv[cp].u = pv2->u1;
			pcv[cp].v = pv2->v1;
			pcv[cp++].y = pv2->y1;
		}
		v1 = v2;
	}
	clip->vertcount = cp;

	// Clip to screen bottom at ymax
	cp=0;
	v1=clip->vertcount-1;
	for(zp_size_t v2=0; v2 < clip->vertcount; v2++)
	{
		pv1=&clip->vertex[v1];
		pv2=&clip->vertex[v2];
		if((pv1->y <= ymax) && (pv2->y <= ymax))
		{
			pcv[cp].x1 = pv2->x;
			pcv[cp].z1 = pv2->z;
			pcv[cp].u1 = pv2->u;
			pcv[cp].v1 = pv2->v;
			pcv[cp++].y1 = pv2->y;
		}
		if((pv1->y <= ymax) && (pv2->y > ymax))
		{
			t=(ymax - pv1->y) / (pv2->y - pv1->y);
			pcv[cp].x1 = pv1->x + (pv2->x - pv1->x) * t;
			pcv[cp].z1 = pv1->z + (pv2->z - pv1->z) * t;
			pcv[cp].u1 = pv1->u + (pv2->u - pv1->u) * t;
			pcv[cp].v1 = pv1->v + (pv2->v - pv1->v) * t;
			pcv[cp++].y1 = ymax;
		}
		if((pv1->y > ymax) && (pv2->y <= ymax))
		{
			t=(ymax - pv1->y) / (pv2->y - pv1->y);
			pcv[cp].x1 = pv1->x + (pv2->x - pv1->x) * t;
			pcv[cp].z1 = pv1->z + (pv2->z - pv1->z) * t;
			pcv[cp].u1 = pv1->u + (pv2->u - pv1->u) * t;
			pcv[cp].v1 = pv1->v + (pv2->v - pv1->v) * t;
			pcv[cp++].y1 = ymax;
			pcv[cp].x1 = pv2->x;
			pcv[cp].z1 = pv2->z;
			pcv[cp].u1 = pv2->u;
			pcv[cp].v1 = pv2->v;
			pcv[cp++].y1 = pv2->y;
		}
		v1 = v2;
	}
	clip->vertcount = cp;

	// Clip to screen left at xmin
	cp=0;
	v1=clip->vertcount-1;

	for(zp_size_t v2=0; v2 < clip->vertcount; v2++)
	{
		pv1=&clip->vertex[v1];
		pv2=&clip->vertex[v2];
		if((pv1->x1 >= xmin) && (pv2->x1 >= xmin))
		{
			pcv[cp].y = pv2->y1;
			pcv[cp].z = pv2->z1;
			pcv[cp].u = pv2->u1;
			pcv[cp].v = pv2->v1;
			pcv[cp++].x = pv2->x1;
		}
		if((pv1->x1 >= xmin) && (pv2->x1 < xmin))
		{
			t=(xmin - pv1->x1) / (pv2->x1 - pv1->x1);
			pcv[cp].y = pv1->y1 + (pv2->y1 - pv1->y1) * t;
			pcv[cp].z = pv1->z1 + (pv2->z1 - pv1->z1) * t;
			pcv[cp].v = pv1->v1 + (pv2->v1 - pv1->v1) * t;
			pcv[cp].u = pv1->u1 + (pv2->u1 - pv1->u1) * t;
			pcv[cp++].x = xmin;
		}
		if((pv1->x1 < xmin) && (pv2->x1 >= xmin))
		{
			t=(xmin - pv1->x1) / (pv2->x1 - pv1->x1);
			pcv[cp].y = pv1->y1 + (pv2->y1 - pv1->y1) * t;
			pcv[cp].z = pv1->z1 + (pv2->z1 - pv1->z1) * t;
			pcv[cp].v = pv1->v1 + (pv2->v1 - pv1->v1) * t;
			pcv[cp].u = pv1->u1 + (pv2->u1 - pv1->u1) * t;
			pcv[cp++].x = xmin;
			pcv[cp].y = pv2->y1;
			pcv[cp].z = pv2->z1;
			pcv[cp].u = pv2->u1;
			pcv[cp].v = pv2->v1;
			pcv[cp++].x = pv2->x1;
		}
		v1 = v2;
	}
	clip->vertcount = cp;

	// Clip to screen right at xmax
	cp=0;
	v1=clip->vertcount-1;
	for(zp_size_t v2=0; v2 < clip->vertcount; v2++)
	{
		pv1=&clip->vertex[v1];
		pv2=&clip->vertex[v2];
		if((pv1->x <= xmax) && (pv2->x <= xmax))
		{
			pcv[cp].y1 = pv2->y;
			pcv[cp].z1 = pv2->z;
			pcv[cp].u1 = pv2->u;
			pcv[cp].v1 = pv2->v;
			pcv[cp++].x1 = pv2->x;
		}
		if((pv1->x <= xmax) && (pv2->x > xmax))
		{
			t=(xmax - pv1->x) / (pv2->x - pv1->x);
			pcv[cp].y1 = pv1->y + (pv2->y - pv1->y) * t;
			pcv[cp].z1 = pv1->z + (pv2->z - pv1->z) * t;
			pcv[cp].u1 = pv1->u + (pv2->u - pv1->u) * t;
			pcv[cp].v1 = pv1->v + (pv2->v - pv1->v) * t;
			pcv[cp++].x1 = xmax;
		}
		if((pv1->x > xmax) && (pv2->x <= xmax))
		{
			t=(xmax - pv1->x) / (pv2->x - pv1->x);
			pcv[cp].y1 = pv1->y + (pv2->y - pv1->y) * t;
			pcv[cp].z1 = pv1->z + (pv2->z - pv1->z) * t;
			pcv[cp].u1 = pv1->u + (pv2->u - pv1->u) * t;
			pcv[cp].v1 = pv1->v + (pv2->v - pv1->v) * t;
			pcv[cp++].x1 = xmax;
			pcv[cp].y1 = pv2->y;
			pcv[cp].z1 = pv2->z;
			pcv[cp].u1 = pv2->u;
			pcv[cp].v1 = pv2->v;
			pcv[cp++].x1 = pv2->x;
		}
		v1 = v2;
	}

	clip->vertcount = cp;

}



inline void RenderTF()
{
	
	float SurfaceMultW;
	float SurfaceMultH;


	BYTE * dibits;

	BYTE x,y;
	unsigned int i;

	float zover;
	float qqqc;

	bool hasmap;

	BYTE faceC[4] = {};

	cTexture *MyTexture;
	BYTE *MyTextureP = NULL;
	// Reasterizer vars
	unsigned int a=0;

	// Daves Rasterizer.
	zp_size_t vertCnt;
	unsigned int TopVr,BotVr,TopVl,BotVl;
	int TopYr,BotYr,TopYl,BotYl;
	float DistanceR;
	float DistanceL;
	float tVal;
	float StepU,StepV,StepZ;
	float StepUL,StepVL,StepZL,SlpL;
	float StepUR,StepVR,StepZR,SlpR;
	float Xl,Xr;
	float Zl,Ul,Vl,Zr,Ur,Vr;

	float tZ,tU,tV;
	float DeltaX;
	float OneOverZ;

	zp_size_t t;

	zp_size_t poly_cnt;
	zp_size_t obj_cnt;
	object_type *curobj;
	polygon_type *curpolygon;


	// Clear ZBuffer if needed 

	//BYTE*					video_bufferC		= NULL;	// used to draw onto back-keyboard_buffer
	//byte video_local[SCREEN_WIDTH*SCREEN_HEIGHT*4];
	//ZeroMemory ( video_local, sizeof ( video_local ));
	//video_bufferC = video_local;
	//delete video_local;

	// For every object in the World
	for(obj_cnt=0;obj_cnt<WLD.objcount;obj_cnt++)
	{

		curobj = &WLD.obj[obj_cnt];	// Pointer to this object.
		if(curobj->hidden) continue;
		if(curobj->scale==0) continue;
		// For every polygon in an object
		total_faces += curobj->polycount;
		for(poly_cnt=0;poly_cnt<curobj->polycount;poly_cnt++)
		{
			curpolygon = &curobj->polygon[poly_cnt];// Pointer to this polygon.

			if(curpolygon->visable>0) continue;	// Calculated in rotate world. >0 indicates not visable.

			if (obj_cnt == 1) {
				int a = 0;
			}

			zclip(curpolygon,&pclip);		// Clip polygon to frontZ and return pclip
			if(pclip.vertcount<3) continue;	// No points left in polygon so move on to next



			for(t=0; t<pclip.vertcount; t++) {  // Project verts to screen space.
				zover = 1/pclip.vertex[t].z;	// zclip will ensure z is never 0
				pclip.vertex[t].x1 = (pclip.vertex[t].x * FOV * zover) + SCREEN_WIDTHh;
				pclip.vertex[t].y1 = (pclip.vertex[t].y * FOV * zover) + SCREEN_HEIGHTh;
				pclip.vertex[t].z1 = zover;
				pclip.vertex[t].u1 = pclip.vertex[t].u * zover;	// uv texture cords.
				pclip.vertex[t].v1 = pclip.vertex[t].v * zover;

			}
			
			tclip(&pclip);	// Clip polygon against screen

			if(pclip.vertcount<3) continue;	// No points left in polygon so move on to next

			// Get Texture details

			MyTexture = &Texture[curpolygon->SurfaceTexture];
			if(MyTexture->bmWidth)
			{
				hasmap = true;
				MyTextureP   = MyTexture->bmpBuffer;
				SurfaceMultH = static_cast<float>(MyTexture->bmHeight);
				SurfaceMultW = static_cast<float>(MyTexture->bmWidth);
			} else {
				hasmap = false;
				faceC[0] = static_cast<BYTE>(MyTexture->faceB * curpolygon->D);
				faceC[1] = static_cast<BYTE>(MyTexture->faceG * curpolygon->D);
				faceC[2] = static_cast<BYTE>(MyTexture->faceR * curpolygon->D);
				faceC[3] = NULL;
			}

			
			////////////////////////// Rasterize.
			TopVr = 0 ; TopYr = static_cast<int>(pclip.vertex[0].y1);
			vertCnt = pclip.vertcount;
			// Find top vertex
			for(a = 0; a<vertCnt; a++)
			{
				if(pclip.vertex[a].y1 < TopYr)
				{
					TopVr = a; TopYr = static_cast<int>(pclip.vertex[a].y1);
				}
			}

			TopVl = TopVr + 1; TopVr = TopVr - 1;

			DistanceR = 0;
			DistanceL = 0;

		    while(1)
			{

				if(!DistanceR)
				{
				NewR:
					if(!vertCnt--) break;
					TopVr++;
					if(TopVr == pclip.vertcount) TopVr = 0;
					if(TopVr == pclip.vertcount-1) BotVr = 0; else BotVr = TopVr + 1;

					TopYr = static_cast<int>(pclip.vertex[TopVr].y1);
					BotYr = static_cast<int>(pclip.vertex[BotVr].y1);

					DistanceR = static_cast<float>(TopYr - BotYr);
					if(!DistanceR) goto NewR;

					Xr = pclip.vertex[TopVr].x1;
					Zr = pclip.vertex[TopVr].z1;
					Ur = pclip.vertex[TopVr].u1;
					Vr = pclip.vertex[TopVr].v1;

					tVal = 1 / DistanceR;
					SlpR = (Xr - pclip.vertex[BotVr].x1) * tVal;
					StepZR = (Zr - pclip.vertex[BotVr].z1) * tVal;
					StepUR = (Ur - pclip.vertex[BotVr].u1) * tVal;
					StepVR = (Vr - pclip.vertex[BotVr].v1) * tVal;
				}


				if(!DistanceL)
				{
				newL:
					if(!vertCnt--) break;
					TopVl--;
					if(TopVl==-1) TopVl = pclip.vertcount - 1;
					if(TopVl==0) BotVl = pclip.vertcount - 1; else BotVl = TopVl - 1;
					
					TopYl = static_cast<int>(pclip.vertex[TopVl].y1);
					BotYl = static_cast<int>(pclip.vertex[BotVl].y1);

					DistanceL = static_cast<float>(TopYl - BotYl);
					if(!DistanceL) goto newL;

					Xl = pclip.vertex[TopVl].x1+1;
					Zl = pclip.vertex[TopVl].z1;
					Ul = pclip.vertex[TopVl].u1;
					Vl = pclip.vertex[TopVl].v1;

					tVal = 1 / DistanceL;
					SlpL = (Xl - pclip.vertex[BotVl].x1) * tVal;
					StepZL = (Zl - pclip.vertex[BotVl].z1) * tVal;
					StepUL = (Ul - pclip.vertex[BotVl].u1) * tVal;
					StepVL = (Vl - pclip.vertex[BotVl].v1) * tVal;
				}
        
				//////////////////////////////////////
		        
				if(DeltaX = Xr - Xl+1)
				{
					DeltaX = 1/DeltaX;
					StepZ = (Zr - Zl) * DeltaX;

					tZ = Zl;

					video_buffer = video_bufferB + (int(Xl)*4) + (TopYr * video_buffer_row_size);
					pZBuffer = ZBuffer + int(Xl) + (TopYr*SCREEN_WIDTH);	

					if(hasmap)
					{

						StepU = (Ur - Ul) * DeltaX * SurfaceMultW;
						StepV = (Vr - Vl) * DeltaX * SurfaceMultH;

						tU = Ul*SurfaceMultW;
						tV = Vl*SurfaceMultH;

						for(i = Xr; i>Xl-1;--i)
						//for(i = Xl; i<Xr;i++)
						{
							if((qqqc = tZ+ZBufferPage) > *pZBuffer)
							{
								*pZBuffer = qqqc;
								OneOverZ = 1 / tZ;
								x = static_cast<BYTE>(tU * OneOverZ);
								y = static_cast<BYTE>(tV * OneOverZ);
								dibits = &MyTextureP[((long)x*4) + ((long)y * MyTexture->bmWidthBytes)];
								video_buffer[3] = static_cast<BYTE>(dibits[0] * curpolygon->D); //
								video_buffer[2] = static_cast<BYTE>(dibits[1] * curpolygon->D);
								video_buffer[1] = static_cast<BYTE>(dibits[2] * curpolygon->D);
							}
							tZ+=StepZ;
							tU+=StepU;
							tV+=StepV;

							video_buffer+=4;
							pZBuffer++;
						}

					} else {

						for(i = Xr; i>Xl-1;--i)
						{
							if((qqqc = tZ+ZBufferPage) > *pZBuffer)
							{
								*pZBuffer = qqqc;
								video_buffer[3] = faceC[0];	// SDL -> 0xAABBGGRR
								video_buffer[2] = faceC[1];
								video_buffer[1] = faceC[2];		


							}

							tZ+=StepZ;
							video_buffer+=4;
							pZBuffer++;
						}

					}

				}

				//////////////////////////////////////
				Xr+=SlpR;
				Zr+=StepZR;
				Ur+=StepUR;
				Vr+=StepVR;

				Xl+=SlpL;
				Zl+=StepZL;
				Ul+=StepUL;
				Vl+=StepVL;
        
				DistanceR++;
				DistanceL++;
				TopYr++;
				TopYl++;
			}
			rendered_faces += 1;
		}

		rendered_objects += 1;
	}
     
	//debug2 = SDL_GetTicks()-debug2;
}






bool LoadTexture(cTexture &MyTexture, const wchar_t *szFileName, bool force256)
{

	// Loads 24 bit Bitmap from file and streches it to fit our
	// standard 256x256 texture memory map. 

	BITMAP        bm;
	HBITMAP       hBitmap;
	HPALETTE      hPalette;
	BYTE		*tmpBMP = 0;

	std::wstring file = PATH_ASSETS;
	file += szFileName;

	if( LoadBitmapFromBMPFile(file.c_str(), &hBitmap, &hPalette))
	{
		GetObject( hBitmap, sizeof(BITMAP), &bm );
/*
		if(MyTexture.bmBitsPixel==24)
		{
			// Only suport 24bit maps at the moment.
			DeleteObject( hBitmap );
			DeleteObject( hPalette );
			return false;
		}
*/
		// Copy important to cTexture structure. (not actualy used yet?)
		MyTexture.bmBitsPixel = bm.bmBitsPixel;
		MyTexture.bmPlanes = bm.bmPlanes;
		MyTexture.bmType = bm.bmType;
		MyTexture.filename = szFileName;

		// Create temp and keyboard_buffer btye arrays for Bitmap data.
		tmpBMP = new BYTE[bm.bmWidth * bm.bmHeight * 3];		// Source size.
		// This is our texture byte array. Why 4 * 256 * 256 ? Well even though.
		// we only need 3 bytes per pixel for RGB it is much faster to get bits
		// out of out texture map this way.. consider the following if we used 3 bytes
		// and any texture Width..
		//		dibits += (x * 3) + (y * TextureWidth * 3);
		// You can calculate that every poly pixel or this faster way.
		//		dibits += (x<<2) + (y<<10);  // for 256
		//		dibits += (x<<2) + (y<<10);  // for 128

		if(force256)
		{
			MyTexture.bmWidth = 256;
			MyTexture.bmHeight = 256;
		} else 
		{
			MyTexture.bmWidth = bm.bmWidth;
			MyTexture.bmHeight = bm.bmHeight;
		}

		MyTexture.bmWidthBytes = MyTexture.bmWidth * 4;
		MyTexture.bmpBuffer= new BYTE[4 * MyTexture.bmWidth * MyTexture.bmHeight];
		
		zp_size_t r = GetBitmapBits(hBitmap, bm.bmWidth * bm.bmHeight * 3,tmpBMP);

		// Converting bitmap to continous byte array.. Saves doing the same
		// sort of math per pixel later on Also we are streching the origional
		// Bitmap to fit 256x256.
		for(float y=0;y<MyTexture.bmHeight;y++)
		{
			for(float x=0;x<MyTexture.bmWidth;x++)
			{
				BYTE * dibitsd = MyTexture.bmpBuffer;
				BYTE * dibits = tmpBMP;

				float xx = (float(bm.bmWidth)/MyTexture.bmWidth)*x;
				float yy = (float(bm.bmHeight)/MyTexture.bmHeight)*y;
				dibits += (long(xx) * 3);
				dibits += (long(yy) * (bm.bmWidth*3));
				if(bm.bmWidth*3==bm.bmWidthBytes-1) dibits += long(yy);
				if(bm.bmWidth*3==bm.bmWidthBytes-3) dibits += long(yy);

				dibitsd += long(long(x) * 4) + long(long(y) * (MyTexture.bmWidth*4));

				dibitsd[0] = dibits[0];
				dibitsd[1] = dibits[1];
				dibitsd[2] = dibits[2];
				dibitsd[3] = 255;
			}
		}
		

		DeleteObject( hBitmap );
		DeleteObject( hPalette );
		delete[] tmpBMP;

		MyTexture.hasAlpha = false;

		return true;
	} else {
		return false;
	}
}



bool LoadTextureAlpha(cTexture &MyTexture, const wchar_t *szFileName, bool force256)
{

	// Loads 24 bit Bitmap from file and streches it to fit our
	// standard 256x256 texture memory map. 
	
	BITMAP        bm;
	HBITMAP       hBitmap;
	HPALETTE      hPalette;
	BYTE		*tmpBMP;
	
	std::wstring absfile = L"C:\\temp\\zspace\\zspace_2025\\zspace\\assets\\";
	absfile += szFileName;

	if( LoadBitmapFromBMPFile(absfile.c_str(), &hBitmap, &hPalette))
	{

		GetObject( hBitmap, sizeof(BITMAP), &bm );

		// Create temp and keyboard_buffer btye arrays for Bitmap data.
		tmpBMP = new BYTE[bm.bmWidth * bm.bmHeight * 3];		// Source size.


		//MyTexture.bmpBuffer= new BYTE[4 * MyTexture.bmWidth * MyTexture.bmHeight];
		GetBitmapBits(hBitmap, bm.bmWidth * bm.bmHeight * 3,tmpBMP);

		// Converting bitmap to continous byte array.. Saves doing the same
		// sort of math per pixel later on Also we are streching the origional
		// Bitmap to fit 256x256.
		for(float y=0;y<MyTexture.bmHeight;y++)
		{
			for(float x=0;x<MyTexture.bmWidth;x++)
			{
				BYTE * dibitsd = MyTexture.bmpBuffer;
				BYTE * dibits = tmpBMP;

				float xx = (float(bm.bmWidth)/MyTexture.bmWidth)*x;
				float yy = (float(bm.bmHeight)/MyTexture.bmHeight)*y;
				dibits += (long(xx) * 3);
				dibits += (long(yy) * (bm.bmWidth*3));
				if(bm.bmWidth*3==bm.bmWidthBytes-1) dibits += long(yy);
				if(bm.bmWidth*3==bm.bmWidthBytes-3) dibits += long(yy);

				dibitsd += long(long(x) * 4) + long(long(y) * (MyTexture.bmWidth*4));
				
				dibitsd[3] = dibits[0];
			}
		}
		

		DeleteObject( hBitmap );
		DeleteObject( hPalette );
		delete[] tmpBMP;

		MyTexture.hasAlpha = true;

		return true;
	} else {
		return false;
	}
}



bool LoadBitmapFromBMPFile(const wchar_t* szFileName, HBITMAP *phBitmap,HPALETTE *phPalette )
{
   BITMAP  bm;

   *phBitmap = NULL;
   *phPalette = NULL;

   //Public Declare Function LoadBitmap Lib "user32" Alias "LoadBitmapA" (ByVal hInstance As Long, ByVal lpBitmapName As String) As Long
	
   // Use LoadImage() to get the image loaded into a DIBSection
   *phBitmap = (HBITMAP)LoadImage( NULL, szFileName, IMAGE_BITMAP, 0, 0,
               LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );

   if( *phBitmap == NULL )
     return false;

   // Get the color depth of the DIBSection
   GetObject(*phBitmap, sizeof(BITMAP), &bm );
   // If the DIBSection is 256 color or less, it has a color table
	/*
   if( ( bm.bmBitsPixel * bm.bmPlanes ) <= 8 )
   {
   HDC           hMemDC;
   HBITMAP       hOldBitmap;
   RGBQUAD       rgb[256];
   LPLOGPALETTE  pLogPal;
   WORD          i;

   // Create a memory DC and select the DIBSection into it
   hMemDC = CreateCompatibleDC( NULL );
   hOldBitmap = (HBITMAP)SelectObject( hMemDC, *phBitmap );
   // Get the DIBSection's color table
   GetDIBColorTable( hMemDC, 0, 256, rgb );
   // Create a palette from the color tabl
   pLogPal = (LOGPALETTE *)malloc( sizeof(LOGPALETTE) + (256*sizeof(PALETTEENTRY)) );
   pLogPal->palVersion = 0x300;
   pLogPal->palNumEntries = 256;
   for(i=0;i<256;i++)
   {
     pLogPal->palPalEntry[i].peRed = rgb[i].rgbRed;
     pLogPal->palPalEntry[i].peGreen = rgb[i].rgbGreen;
     pLogPal->palPalEntry[i].peBlue = rgb[i].rgbBlue;
     pLogPal->palPalEntry[i].peFlags = 0;
   }
   *phPalette = CreatePalette( pLogPal );
   // Clean up
   free( pLogPal );
   SelectObject( hMemDC, hOldBitmap );
   DeleteDC( hMemDC );
   }
   else   // It has no color table, so use a halftone palette
   {
   */
   HDC    hRefDC;

   hRefDC = GetDC( NULL );
   *phPalette = CreateHalftonePalette( hRefDC );
   ReleaseDC( NULL, hRefDC );
   //}
   return true;

}







