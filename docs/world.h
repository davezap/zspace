/*
	 zspace code.
	 Daves 3D thing, Copy and Credit me, thanks. 

Copyright (c) 2002-2006, David Chamberlain, I did it.
All rights reserved. And so on and so on.
email me. bobason456@hotmail.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, 
      this list of conditions and the following disclaimer in the documentation 
      and/or other materials provided with the distribution.
    * The name of the author may be used to endorse or promote products 
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
THE POSSIBILITY OF SUCH DAMAGE.
*/


//const unsigned int SCREEN_WIDTH = 320, SCREEN_HEIGHT = 200;
//const unsigned int SCREEN_WIDTH = 320, SCREEN_HEIGHT = 240;
//const unsigned int SCREEN_WIDTH = 400, SCREEN_HEIGHT = 300;

	//const unsigned int SCREEN_WIDTH = 512, SCREEN_HEIGHT = 384;

//const unsigned int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 400;
//const unsigned int SCREEN_WIDTH = 640, SCREEN_HEIGHT = 480;
//const unsigned int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;
const unsigned int SCREEN_WIDTH = 1024, SCREEN_HEIGHT = 768;
const unsigned int SCREEN_WIDTHh = SCREEN_WIDTH/2;
const unsigned int SCREEN_HEIGHTh = SCREEN_HEIGHT/2;

const unsigned int mines_cnt = 100;

//float LS[3][7];
float const Rad = 3.141592654 / 180;    //multiply degrees by Rad to get angle in radians
float LocalSIN[360];
float LocalCOS[360];

//float iX,iY,iZ,oX,oY,oZ;	// intersection returns

int an_X,an_Y,an_Z;	// View Point Angle
float CamX,CamY,CamZ;	// View Point Pos
float CamZoomY;

int CamOb;

long ShotSprite;
long ShrapSprite;
long BoomSprite;
long ShotSprite_len;
long ShrapSprite_len;
long BoomSprite_len;

const float TMS=300;				// fp distance from screen

bool AltRender = false;
bool AltSprite = false;
bool RotOb = false;

float scale = 2;

struct cTexture
{
	char filename[255];
	long bmBitsPixel;
	unsigned int bmHeight;
	long bmPlanes;
	long bmType;
	unsigned int bmWidth;
	unsigned int bmWidthBytes;
	BYTE *bmpBuffer;
	unsigned int faceR,faceG,faceB;
	bool hasAlpha;
};

cTexture Texture[200];
int TextureCnt;

struct sprite
{
	float x;
	float y;
	float z;
	float lx;
	float ly;
	float lz;
	float vx,vy,vz;
	bool p;
	bool visable;
	int liveTime;
	bool distance_infinity;
	bool zbuffer;
	bool scale;
	int animation_start;
	int animation_len;
	float alpha;
	cTexture *boTexture;
};

sprite *sprites_back;
int sprites_back_cnt;
sprite *sprites_front;
int sprites_front_cnt;
sprite *sprite_tmp;

long SelTxr;
long chk_TextureShow;

float ZBuffer[SCREEN_WIDTH*SCREEN_HEIGHT];
float *pZBuffer;
long ZBuffer_Offset;
long ZBufferPage=0;

int TmpScreen [SCREEN_WIDTH*SCREEN_HEIGHT*4];

struct vertex_type
{
	float lx,ly,lz,lw;
	float wx,wy,wz;
	float sx,sy;
	float u,v;
};

struct uv_type
{
	float u,v;
};

struct polygon_type 
{
    float nX;
    float nY;
    float nZ;
	float D;
	long SurfaceTexture;

	vertex_type *vertex[3];
	uv_type uv[3];

	int visable;
};

struct object_type
{
	unsigned int vertcount;
	unsigned int polycount;
	float scale;
	float x;
	float y;
	float z;
	float wx;
	float wy;
	float wz;
	int an_X,an_Y,an_Z;
	float vax;
	float vay;
	float vaz;
	float thrust,turnpitch;
	float vx;
	float vy;
	float vz;
	bool shoot;
	polygon_type *polygon;
	vertex_type *vertex;

	bool hidden;
};

object_type *camobj;
object_type *curobj;


struct world_type
{
	unsigned int objcount;
	object_type *obj;
};


world_type WLD;


struct clip_type
{
	float x,y,z;
	float x1,y1,z1;
	float u,v;
	float u1,v1;
};

struct cliped_polygon_type 
{
	unsigned int vertcount;
	clip_type vertex[8];
};


cliped_polygon_type pclip;


double globalprint;
double debug1;
double debug2;
double debug3;

