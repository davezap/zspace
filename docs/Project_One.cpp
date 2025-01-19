/*
	 zspace code.
	 Daves 3D thing

Copyright (c) 2002-2006, DaveZ - 
All rights reserved. And so on and so on.
email me. DaveZ@204am.com

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

// SYSTEM-DEFINES /////////////////////////////////////////////////////////////////////////////////

#define D3D_OVERLOADS
#define WIN32_LEAN_AND_MEAN
#define INITGUID


// INCLUDES ///////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <math.h>
#include <fstream>

#include "D:\DXSDK\Include\ddraw.h"
#include "D:\DXSDK\Include\dinput.h"

#include <Stdio.h>
#include <Mmsystem.h>

#include "world.h"

#include "lx.h"

// APP DEFINES ////////////////////////////////////////////////////////////////////////////////////


// USING DECLARATION //////////////////////////////////////////////////////////////////////////////

using namespace std;


// DATA STRUCTURES ////////////////////////////////////////////////////////////////////////////////


// FUNCTION PROTOTYPES ////////////////////////////////////////////////////////////////////////////

char* DI_Start ( void );
char* DD_Start ( void );
void DI_End ( void );
void DD_End ( void );
char* Game_Main ( void );
void ProcessInput ( void );
void UpdateVariables ( void );
inline void Boom(int x,int y,bool cast_shrap,bool force);
inline void Shrap(int x,int y,int sh_type,int scnt);
//inline void GetFreeSprite () long;
inline long GetFreeSprite (bool force);


inline void DrawScreen();
bool OnIdle(LONG lCount);
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void ErrorMessage(LPCSTR lpText, HRESULT hResult);


// World Function prototypes /////////////////////////////////////

void SetUpWorld(void);
void DestroyWorld(void);

inline void MatMult(float mat1[4][4], float mat2[4][4]);
inline void Matrix_Ident(float Matrix[4][4]);
inline void RotatePntExB(float Matrix[4][4], int AngX, int AngY, int AngZ);
inline void RotatePntExBe(float Matrix[4][4], int AngX, int AngY, int AngZ);
inline void RotateAxis(float Matrix[4][4], int Ang, float u, float v, float w);
inline void Translate(float Matrix[4][4], float &Xt, float &Yt, float &Zt);
inline void Transform(float Matrix[4][4], float &pX, float &pY, float &pZ);


inline void CalcNorm(polygon_type &Poly);

inline void RenderSprites(sprite objs[], int &objCnt);
inline void RenderSprites2(void);
inline void RenderSprites3(void);
inline void RenderTF();
inline void RenderW();

inline void ClearScreen();

inline void FixAngle(int &angle);
inline float MyCos(int &angle);
inline float MySin(int &angle);

//bool LoadTexture(cTexture &MyTexture, LPTSTR szFileName);
bool LoadTexture(cTexture &MyTexture, LPTSTR szFileName, bool force256);
bool LoadTextureAlpha(cTexture &MyTexture, LPTSTR szFileName, bool force256);
bool LoadBitmapFromBMPFile( LPTSTR szFileName, HBITMAP *phBitmap,HPALETTE *phPalette );
inline RGBQUAD GetPixel(cTexture &MyTexture, float x, float y);

inline void RotateWorld(void);
inline void MeshLights();
//inline float CosAngle(float AX, float AY, float AZ, float bx, float by, float bz);
inline float CosAngle(float &AX, float &AY, float &AZ, float &bx, float &by, float &bz);

void Test(void);
// GLOBALS ////////////////////////////////////////////////////////////////////////////////////////

HWND					g_hwnd				= NULL;		// global window handle
HINSTANCE				g_hinst				= NULL;		// global instance

char					str[255]			= "";
char*					error_string		= NULL;		// Used to store error messages

// DIRECT DRAW (VERSION 7) ////////////////////////////////////////////////////////////////////////

LPDIRECTDRAW7			gpDD				= NULL;	// DirectDraw Object
LPDIRECTDRAWSURFACE7	gpDDSPrimary		= NULL;	// DirectDraw Primary surface
LPDIRECTDRAWSURFACE7	gpDDSBackBuffer		= NULL;	// DirectDraw Back-buffer surface
LPDIRECTDRAWSURFACE7	gpDDSDraw			= NULL;	// DirectDraw Off-Screen surface
//LPDIRECTDRAWSURFACE7	gpDDSDigits			= NULL; // DirectDraw Z-Buffer surface (software)

DDBLTFX					ddbltfx;					// Blitter object

BYTE*					video_buffer		= NULL;	// used to draw onto back-buffer
BYTE*					video_bufferB		= NULL;	// used to draw onto back-buffer

// DIRECT INPUT (VERSION 8) ///////////////////////////////////////////////////////////////////////

LPDIRECTINPUT8			gpDI;						// Main DInput object
LPDIRECTINPUTDEVICE8	gpDIDevice;					// Keyboard device

UCHAR					buffer [256];
int						words_per_line		= 0;

int ccc=0;	
int ddd=1;	

long fps_Frames=0;
DWORD fps_Time=0;

// FUNCTIONS //////////////////////////////////////////////////////////////////////////////////////

void Test(void)
{
//Test Int
   int i = 0, sum = 0;
   const int max = 10;
   i = 1;
//Test 1
loop:
   sum += i;             // Add current value of i to sum
   if(++i <= max)
      goto loop;         // Go back to loop until i = 11
//Test 2
for(sum=0;sum < max ; sum+=i)
{
}
// end 

}

// WindowProc() ///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// Handles windows messages ///////////////////////////////////////////////////////////////////////

LRESULT CALLBACK WindowProc ( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{

	switch ( msg )
	{	
		case WM_DESTROY: 
		{
			PostQuitMessage(0);
			return(0);
		};

		default:
			break;

	}

	return ( DefWindowProc ( hwnd, msg, wparam, lparam ));

}


// WinMain () /////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// Program execution starts here //////////////////////////////////////////////////////////////////

int WINAPI WinMain(	HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{

	WNDCLASS	wcl;	// window class
	HWND		hwnd;	// local window handle
	MSG			msg;	// message


	// Complete Window Class Struct
	wcl.style			= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	wcl.lpfnWndProc		= WindowProc;
	wcl.cbClsExtra		= 0;
	wcl.cbWndExtra		= 0;
	wcl.hInstance		= hinstance;
	wcl.hIcon			= LoadIcon ( NULL, IDI_APPLICATION );
	wcl.hCursor			= LoadCursor ( NULL, IDC_ARROW );
	wcl.hbrBackground	= (HBRUSH) GetStockObject ( BLACK_BRUSH );
	wcl.lpszMenuName	= NULL; 
	wcl.lpszClassName	= "DDClass";

	// Register the Class
	if ( !RegisterClass ( &wcl ))
	{
		return ( 0 );
	}


	// Create the Window
	if (!(hwnd = CreateWindow("DDClass",				// Class
							  "DD8Console",				// Title
							  WS_POPUP | WS_VISIBLE,	// Style flags
						 	  0,0,						// Location x,y
							  SCREEN_WIDTH,				// Width
						      SCREEN_HEIGHT,			// Height
							  NULL,						// Handle to parent Window
							  NULL,						// Handle to menu
							  hinstance,				// Handle to instance
							  NULL )))					// Misc. parameterss
	{
		return ( 0 );
	}

	
	// Save the window handle and instance
	g_hwnd  = hwnd;
	g_hinst = hinstance;


	// DirectX Specific Initialization
	if ( error_string == NULL ) error_string = DI_Start ();
	if ( error_string == NULL ) error_string = DD_Start ();

	
	// Check for Initialization errors
	if ( error_string != NULL )
	{
		MessageBox	( g_hwnd, error_string, "Fatal Error!", MB_OK | MB_ICONSTOP );
		PostMessage ( g_hwnd, WM_DESTROY, 0, 0 );
	}


	SetUpWorld();
	fps_Time=timeGetTime();// timeGetTime();
	// Main Message Loop
	while ( true )
	{
	
		if ( PeekMessage ( &msg, NULL, 0, 0, PM_REMOVE ))
		{ 
			// See if we have a Quit message
			if (msg.message == WM_QUIT)
			{

				break;
			}
	
			// Translate accelerator keys
			TranslateMessage(&msg);

			// Send message for processing by WindowProc()
			DispatchMessage(&msg);
		}
    
		// Run Main Game Loop
		error_string = Game_Main();

		// Check to see if main loop produced an error
		if ( error_string != NULL )
		{
			MessageBox	( g_hwnd, error_string, "Run-Time Error", MB_OK | MB_ICONSTOP );
			PostMessage ( g_hwnd, WM_DESTROY, 0, 0 );
		}

	}
	
	DestroyWorld();

	
	// Release DirectX specific resources
	DD_End ();
	DI_End ();

	
	

	//delete WLD;   /// destroy this ???

	// Return to Windows
	return ( msg.wParam );

} 


// DI_Start () ////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// This function initializes DirectInput //////////////////////////////////////////////////////////

char* DI_Start ( void )
{

	// Obtain pointer to DI Interface
	if ( DI_OK != DirectInput8Create ( g_hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&gpDI, NULL ))
	{
		return ( "Obtain DI Interface Failed." );
	}


	// Create DI Device for Keyboard
	if ( DI_OK != ( gpDI->CreateDevice(GUID_SysKeyboard, &gpDIDevice, NULL)))
	{
		return ( "Create DI Keyboard Device Failed." );
	}
 

	// Set the Keyboard Data Format
	if ( DI_OK != ( gpDIDevice->SetDataFormat ( &c_dfDIKeyboard )))
	{
		return ( "Set Keyboard Data Format Failed." );
	}
 

	// Set the Keyboard Co-operative Level
	if ( DI_OK != ( gpDIDevice->SetCooperativeLevel ( g_hwnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND )))
	{
		return ( "Set Keyboard Co-Operative Level Failed." );
	}
 

	// Aquire access to the Keyboard
	if ( DI_OK != ( gpDIDevice->Acquire() ))
	{
		return ( "Aquire Keyboard Failed." );
	}


	// Return success
	return ( NULL );

}


// DD_Start () ////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// This function initializes DirectDraw ///////////////////////////////////////////////////////////

char* DD_Start ( void )
{

	// Create the DirectDraw Object
	if ( DD_OK != ( DirectDrawCreateEx ( NULL, (VOID**) &gpDD, IID_IDirectDraw7, NULL )))
	{
		return ( "Create DD Object Failed." );
	}


	// Set the Co-operative level for DirectDraw
	if ( DD_OK != ( gpDD->SetCooperativeLevel ( g_hwnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT )))
	{
		return ( "Set DD Co-operative Level Failed." );
	}

	//#define SCREEN_WIDTH        320
	//#define SCREEN_HEIGHT       200
	//#define SCREEN_BPP          24
	// Set the Display Mode
	if ( DD_OK != ( gpDD->SetDisplayMode ( SCREEN_WIDTH,SCREEN_HEIGHT, 32, 0, 0 )))
	{
		return ( "Set Display Mode Failed." );
	}
	

	// Complete a DDSURFACEDESC2 struct for the Primary Surface
	DDSURFACEDESC2 ddsd;
	ZeroMemory ( &ddsd, sizeof ( DDSURFACEDESC2 ));

	ddsd.dwSize				= sizeof ( DDSURFACEDESC2 );
	ddsd.dwFlags			= DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps		= DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount	= 2;


	// Create the Primary Surface
	if ( DD_OK != ( gpDD->CreateSurface ( &ddsd, &gpDDSPrimary, NULL )))
	{
		return ( "Create Primary Surface Failed." );
	}


	// Get Address of Back Buffer
	DDSCAPS2 ddscaps;
	ZeroMemory ( &ddscaps, sizeof ( DDSCAPS2 ));
	ddscaps.dwCaps	= DDSCAPS_BACKBUFFER;
	
	if ( DD_OK != ( gpDDSPrimary->GetAttachedSurface ( &ddscaps, &gpDDSBackBuffer )))
	{
		return ( "Get Back-buffer Address failed." );
	}
	

	// Complete a BltFX struct
	ZeroMemory ( &ddbltfx, sizeof ( ddbltfx ) );
	ddbltfx.dwSize		= sizeof ( ddbltfx );
	ddbltfx.dwFillColor	= 0;

	// Return success
	return ( NULL );

} 


// DI_End () //////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// This function releases DirectInput Objects /////////////////////////////////////////////////////

void DI_End ( void )
{

	// Release DI Keyboard Device
	if ( gpDIDevice != NULL )
	{
		gpDIDevice->Unacquire();
		gpDIDevice->Release();
		gpDIDevice = NULL;
	}


	// Release DI Interface pointer
	if ( gpDI != NULL )
	{
		gpDI->Release();
		gpDI = NULL;
	}

}


// DD_End () //////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// This function releases DirectDraw Objects //////////////////////////////////////////////////////

void DD_End ( void )
{

	// Release the Primary Surface
	if ( gpDDSPrimary != NULL )
	{
		gpDDSPrimary->Release();
		gpDDSPrimary = NULL;
	}


	// Release the DirectDraw Object
	if ( gpDD != NULL )
	{
		gpDD->Release();
		gpDD = NULL;
	}

}



// Game_Main () ///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// Main game loop /////////////////////////////////////////////////////////////////////////////////

char* Game_Main ( void )
{

	// Retrieve Keyboard Input
	if ( DI_OK != ( gpDIDevice->GetDeviceState ( sizeof (buffer), buffer )))
	{
		return ( "Retrieve Keyboard Input Failed." );
	} 
	//DDBLTFX_MIRRORLEFTRIGHT   DDBLT_ALPHADEST   DDBLT_ALPHASRC
	gpDDSBackBuffer->Blt ( NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx );
	ddbltfx.dwFillColor	= 0;
	//ddbltfx.dwFillColor	= RGB(128,128,0);
	//gpDDSBackBuffer->Blt ( NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx );
	/*
	if ( FAILED ( gpDDSBackBuffer->Blt ( NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx )))
	{
		return ( "Clear Draw Surface with Blit Failed." );
	}
	*/
	
	DDSURFACEDESC2	ddsd; // DirectDraw Surface Description
	//ZeroMemory ( &ddsd, sizeof ( DDSURFACEDESC2 ));
	ddsd.dwSize = sizeof ( DDSURFACEDESC2 );

	// Lock Draw Surface
	if ( FAILED ( gpDDSBackBuffer->Lock ( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL )))
	//if ( FAILED ( gpDDSBackBuffer->Lock ( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR, NULL )))
	{
		return ( "Lock Draw Surface failed." );
	}


	// Store pointer to video-memory
	video_buffer = (BYTE *)ddsd.lpSurface;
	video_bufferB = video_buffer;
	// Store vertical pitch of BackBuffer surface
	words_per_line = (ddsd.lPitch );
	
	//byte* mscreen[sizeof(video_buffer)];
	//memset(mscreen,128,sizeof(ms\creen));
	//memset(mscreen,sizeof(mscreen),sizeof(video_buffer));
	//video_buffer = mscreen;
	/////////////////////////////////////////////
	////// Begin Render /////////////////////////


	RotateWorld();

	// Clear ZBuffer if needed 
	if(ZBufferPage==100)
	{
		ZBufferPage = 1;
		memset(ZBuffer,1,sizeof(ZBuffer));
	} else ZBufferPage++;

	RenderSprites(sprites_back, sprites_back_cnt);
	if(AltSprite) RenderSprites3(); else RenderSprites2();
	RenderTF();
	
  
	/////////////////////////////////////////////
	////// End Render /////////////////////////

	// Process keyboard input
	ProcessInput();

	
	// Maintain variables within limits
	UpdateVariables ();


	// Unlock Draw Surface, we have finished drawing onto it
	if ( FAILED ( gpDDSBackBuffer->Unlock ( NULL )))
	{
		return ( "Unlock Draw Surface Failed." );
	}


	fps_Frames++;
	HDC hdc;
	if (gpDDSBackBuffer->GetDC(&hdc) == DD_OK)  
	{  
		SetBkMode(hdc, TRANSPARENT); 
		SetTextColor(hdc, RGB(255,255,0)); 
		char buffer[32];
		double tmr,tmr2;
		//fps_Frames = 1;
		tmr = timeGetTime()-fps_Time;
		tmr2 = tmr;

		//while(tmr<40)
		//{
		// 	tmr = timeGetTime()-fps_Time;
		//}

		//fps_Time=timeGetTime();

		if(tmr!=0){
			sprintf(buffer, "%3.2f fps, %f", (fps_Frames * 1000) / tmr ,  (fps_Frames * 1000) / tmr2 ); //globalprint
		} else {
			sprintf(buffer, "%f", globalprint   ); 
		}
		
		
		TextOut(hdc, 0, 0, buffer, strlen(buffer)); 
		sprintf(buffer, "%f - %f - %f", debug1,debug2,debug3); 
		debug1 = 0;
		debug2 = 0;
		debug3 = 0;
		TextOut(hdc, 0, 12, buffer, strlen(buffer)); 
		gpDDSBackBuffer->ReleaseDC(hdc);  
	}


	// Flip BackBuffer to Primary surface
	//DDFLIP_ODD,DDFLIP_NOVSYNC,DDFLIP_DONOTWAIT  - DDFLIP_WAIT
	if ( FAILED ( gpDDSPrimary->Flip ( NULL, DDFLIP_WAIT )))
	{
		return ( "Flip BackBuffer to Primary Failed" );
	}


	// Return success
	return ( NULL );

}


// ProcessInput () ////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// This function checks for input and updates variables ///////////////////////////////////////////
int selBone = 0;

void ProcessInput ( void )
{
	#define KEYDOWN(name, key) (name[key] & 0x80) 

	// Escape key to Quit
	if ( buffer[DIK_ESCAPE]		& 0x80 )	PostMessage ( g_hwnd, WM_DESTROY, 0, 0 );

	if ( buffer[DIK_E]		& 0x80 )	 AltRender = false;
	if ( buffer[DIK_R]		& 0x80 )	 AltRender = true;

	if ( buffer[DIK_T]		& 0x80 )	 AltSprite = false;
	if ( buffer[DIK_Y]		& 0x80 )	 AltSprite = true;

	if ( buffer[DIK_Q]		& 0x80 ){fps_Frames=0;fps_Time=timeGetTime();}

	if ( buffer[DIK_W]		& 0x80 )	CamZoomY=0;
	if ( buffer[DIK_Z]		& 0x80 )	CamZoomY+=10;
	if ( buffer[DIK_X]		& 0x80 )	 CamZoomY-=10;
	if ( buffer[DIK_A]		& 0x80 )	CamZoomY+=1;
	if ( buffer[DIK_S]		& 0x80 )	 CamZoomY-=1;

	
	if ( buffer[DIK_B]		& 0x80 )	an_X=-90;
	if ( buffer[DIK_N]		& 0x80 )	an_X++;
	if ( buffer[DIK_M]		& 0x80 )	an_X--;

	if ( buffer[DIK_G]		& 0x80 )	an_Y++;
	if ( buffer[DIK_H]		& 0x80 )	an_Y--;
	
	if ( buffer[DIK_J]		& 0x80 )	an_Z++;
	if ( buffer[DIK_K]		& 0x80 )	an_Z--;

	if ( buffer[DIK_C]		& 0x80 )	Shrap(WLD.obj[CamOb].x,WLD.obj[CamOb].z,1,100);

	if ( buffer[DIK_RIGHT]		& 0x80 )
	{
		WLD.obj[CamOb].turnpitch = 2;
		//WLD.obj[CamOb].an_Y+=6;
	}
	if ( buffer[DIK_LEFT]		& 0x80 )	
	{
		WLD.obj[CamOb].turnpitch = -2;
		//WLD.obj[CamOb].an_Y-=6;
	}
	if(WLD.obj[CamOb].an_Y>359) WLD.obj[CamOb].an_Y = WLD.obj[CamOb].an_Y - 360;
	if(WLD.obj[CamOb].an_Y<0) WLD.obj[CamOb].an_Y = 360 + WLD.obj[CamOb].an_Y;
	//if(WLD.obj[0].an_Z>359) WLD.obj[0].an_Z = WLD.obj[0].an_Z - 360;
	//if(WLD.obj[0].an_Z<0) WLD.obj[0].an_Z = 360 + WLD.obj[0].an_Z;

	
	if ( buffer[DIK_SPACE]			& 0x80 )	 
	{
		WLD.obj[CamOb].shoot = true;
	}

	if ( buffer[DIK_UP]			& 0x80 )	 
	{
		WLD.obj[CamOb].thrust = 1;
	}

	if ( buffer[DIK_DOWN]		& 0x80 )
	{
		WLD.obj[CamOb].thrust = -1;
	}

	/*
	if ( buffer[DIK_RALT]		& 0x80 )	input_scale *= 3;
	if ( buffer[DIK_EQUALS]		& 0x80 )	cube.zPos   += ( 3 * input_scale );
	if ( buffer[DIK_MINUS]		& 0x80 )	cube.zPos   -= ( 3 * input_scale );
	if ( buffer[DIK_LBRACKET]	& 0x80 )	speed -= input_scale;
	if ( buffer[DIK_RBRACKET]	& 0x80 )	speed += input_scale;
*/

}


// UpdateVariables () /////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
// This function maintains certain variables within limits ////////////////////////////////////////
void Boom(int x,int y,bool cast_shrap,bool force)
{
	float tx;
	float tz;
	for(int tmp=0;tmp < sprites_front_cnt;tmp++)
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
	sprites_front[smObj].y = -rand()%30;
	sprites_front[smObj].z = y;
	sprites_front[smObj].vx = 0;
	sprites_front[smObj].vz = 0;
	sprites_front[smObj].vy = 0;
	sprites_front[smObj].liveTime = sprites_front[smObj].animation_len;
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
	long smObj = -1;
	for(int tmp=0;tmp < sprites_front_cnt;tmp++)
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
void Shrap(int x,int y,int sh_type,int scnt)
{
long smObj;
for(int t=1;t<scnt;t++)
{
	smObj = GetFreeSprite(false);
	if(smObj==-1) return;
	//debug3 = sprites_front[tmp].visable;
	sprites_front[smObj].visable = true;
	sprites_front[smObj].x =  x;
	sprites_front[smObj].z =  y;
	sprites_front[smObj].y =  0;
	int ay = rand()%360;
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
	object_type *curobj;
	
	long smObj;
	if(camobj->shoot)
	{
		camobj->shoot = false;
		int ay = camobj->an_Y;
		smObj = GetFreeSprite(true);
		sprite_tmp = &sprites_front[smObj];
		if(smObj==-1) return;
		//debug3 = sprites_front[tmp].visable;
		sprite_tmp->visable = true;
		sprite_tmp->x =  camobj->x;
		sprite_tmp->z =  camobj->z;
		sprite_tmp->y =  0;
		sprite_tmp->vz = (MyCos(ay)/10 * 50)+camobj->vz;
		sprite_tmp->vx = (MySin(ay)/10 * 50)-camobj->vx;
		sprite_tmp->vy = 0;
		sprite_tmp->liveTime = 190;
		sprite_tmp->animation_start = ShotSprite;
		sprite_tmp->animation_len = ShotSprite_len;
		sprite_tmp->boTexture = &Texture[ShotSprite];
		sprite_tmp->alpha = 1;
	}

	float tx;
	float tz;

	//curobj = &WLD.obj[1];
	//curobj->vx = (camobj->x-curobj->x)/200;
	//curobj->vz = (camobj->z-curobj->z)/200;
	
	for(int ob=0;ob<WLD.objcount;ob++)
	{
		curobj = &WLD.obj[ob];
		if(ob!=CamOb)
		{
			//curobj->vx = (camobj->x-curobj->x)/200;
			//curobj->vz = (camobj->z-curobj->z)/200;
			
			curobj->an_X += curobj->vax;
			FixAngle(curobj->an_X);
			curobj->an_Y += curobj->vay;
			FixAngle(curobj->an_Y);
			curobj->an_Z += curobj->vaz;
			FixAngle(curobj->an_Z);
			curobj->x += curobj->vx;
			curobj->y += curobj->vy;
			curobj->z += curobj->vz;
			
			if(curobj->scale<1 && curobj->scale!=0) {
				curobj->scale=0;
				//curobj->y = -1000;
				//if(!curobj->hidden) 
				Boom(curobj->x,curobj->z,true,true);
				Shrap(curobj->x,curobj->z,1,30);
				Shrap(curobj->x,curobj->z,0,30);
				
			}
		}
		
		if(curobj->scale!=0)
		{

			for(int obb=ob+1;obb<WLD.objcount;obb++)
			{
				if (WLD.obj[obb].scale!=0)
				{
				tx = WLD.obj[obb].x - curobj->x;
				tz = WLD.obj[obb].z - curobj->z;
				//if(ob==1 || obb==2) debug3 = sqrt((tx * tx) + (tz * tz));
				if(sqrt((tx * tx) + (tz * tz)) < (15 * curobj->scale * WLD.obj[obb].scale))
				{
					if(ob!=CamOb){
						curobj->scale-=static_cast<float>(0.01);
						//curobj->vx = -curobj->vx;
						//curobj->vz = -curobj->vz;
						WLD.obj[obb].scale-=static_cast<float>(0.01);
						//WLD.obj[obb].vx = -WLD.obj[obb].vx;
						//WLD.obj[obb].vz = -WLD.obj[obb].vz;
						

					} //else Boom(camobj->x,camobj->z,false,false);
					
					//Boom(curobj->x,curobj->z,false,false);
					if(ob==CamOb){
						ddbltfx.dwFillColor	= RGB(0,0,100);
					}
					//break;
				}
				}
			}

			if(ob!=CamOb)
			{
				for(int a = 0;a < sprites_front_cnt;a++)
				{
					sprite_tmp = &sprites_front[a];
					if(sprite_tmp->visable == true && sprite_tmp->animation_start != BoomSprite)
					{
						tx = curobj->x - sprite_tmp->x; 
						tz = curobj->z - sprite_tmp->z;
						if(sqrt((tx * tx) + (tz * tz)) < (15 * curobj->scale))
						{
							sprites_front[a].visable = false;
							if(sprite_tmp->animation_start == ShotSprite)
							{
								tx = sprite_tmp->x;
								tz = sprite_tmp->z;
								curobj->scale -=static_cast<float>(0.05);
								//if(!curobj->hidden) Shrap(tx,tz,0,5);// Boom(tx,tz,true,true);
								//if(!curobj->hidden)
								Boom(tx,tz,true,true);
							}
							
							
						}

					}
				}
			}
		}
	}

	if(camobj->thrust!=0)
	{
		int ay = camobj->an_Y;
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
	Texture[0].faceR = 255 * abs(camobj->thrust);
	Texture[5].faceR = Texture[0].faceR;
	Texture[5].faceG = 0;
	if(Texture[0].faceR>255) Texture[0].faceR=255;


	camobj->thrust = 0;

	if(camobj->turnpitch)
	{
		camobj->an_Z+=camobj->turnpitch;
		if(camobj->an_Z>45) camobj->an_Z=45;
		if(camobj->an_Z<-45) camobj->an_Z=-45;
		camobj->turnpitch = 0;
	} else {
		if(camobj->an_Z!=0) camobj->an_Z/=1.05;
	}
	
	camobj->an_Y+=camobj->an_Z/5;


	CamY = -((sqrt(camobj->vx*camobj->vx + camobj->vz*camobj->vz)*10)+200+CamZoomY);
	CamX = camobj->x;
	CamZ = camobj->z;

	
	CamZ = camobj->z - (-CamY * MyCos(an_X));
	CamY = camobj->y - (CamY * MySin(an_X));

	
	for(int a = 0;a < sprites_front_cnt;a++)
	{
		sprite_tmp = &sprites_front[a];
		if(sprite_tmp->visable == false) continue;
		
		sprite_tmp->liveTime--;
		if(sprite_tmp->liveTime < 0)
		{
			sprite_tmp->visable=false;
			sprite_tmp->liveTime=0;
			continue;
		}
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


	}
	
}



inline void RenderSprites(sprite objs[], int &objCnt)
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

	for(int tmp=0;tmp<objCnt;tmp++)
	{
		sprite_tmp = &objs[tmp];
		
		if(!sprite_tmp->visable) continue;

		if(sprite_tmp->p)
		{
			sprite_tmp->lz = 1/sprite_tmp->lz;
			sprite_tmp->lx = ((sprite_tmp->lx) * sprite_tmp->lz * TMS) + SCREEN_WIDTHh;
			sprite_tmp->ly = ((sprite_tmp->ly) * sprite_tmp->lz * TMS) + SCREEN_HEIGHTh;
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
		videoXOff = sprite_tmp->lx - ThisTexture->bmWidth / 2 ;
		videoYOff = sprite_tmp->ly - ThisTexture->bmHeight / 2 ;

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
			video_buffer = &video_bufferB[(0 + videoYOff) * words_per_line + videoXOff];
			pZBuffer = &ZBuffer[(0 + videoYOff) * SCREEN_WIDTH + videoXOffb];	
			for(int y = 0; y < Vrundistance; y++)
			{
				//video_buffer = &video_bufferB[(y + videoYOff) * words_per_line + videoXOff];
				//pZBuffer = &ZBuffer[(y + videoYOff) * SCREEN_WIDTH + videoXOffb];	
				MyTextureP+=runstart;

				for(int x = 0; x < rundistance; x++)
				{ 
					if(MyTextureP[0] + MyTextureP[1] + MyTextureP[2])
					{
						if((qqqc = sprite_tmp->lz+ZBufferPage) > pZBuffer[0])
						{
							pZBuffer[0] = qqqc;	
							video_buffer[0] = MyTextureP[0];
							video_buffer[1] = MyTextureP[1];
							video_buffer[2] = MyTextureP[2];

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
				video_buffer = video_bufferB + (y+ videoYOff) * words_per_line + videoXOff;
				MyTextureP+=runstart;

				for(int x = 0; x<rundistance;x++)
				{
					video_buffer[0] = MyTextureP[0];
					video_buffer[1] = MyTextureP[1];
					video_buffer[2] = MyTextureP[2];

					video_buffer+=4;
					MyTextureP+=4;
				}

				MyTextureP+=runtostart;
			}
		}


	}


}



inline void RenderSprites2(void)
{
//debug1 = timeGetTime();
//debug1 = timeGetTime();


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

for(int tmp=0;tmp<sprites_front_cnt;tmp++)
{
	
	obj = &sprites_front[tmp];

	if(!obj->visable) continue;
	obj->lz = 1/obj->lz;
	obj->lx = ((obj->lx) * obj->lz * TMS) + SCREEN_WIDTHh;
	obj->ly = ((obj->ly) * obj->lz * TMS) + SCREEN_HEIGHTh;
	if(obj->lx < 0 || obj->lx>SCREEN_WIDTH || obj->ly < 0 || obj->ly>SCREEN_HEIGHT) continue;
	if(obj->lz<0)continue;

	// Get pointer to this background objects texture
	ThisTexture = obj->boTexture;
	//\scale = 2;
	// Get video offset of this sprite
	offset_x = obj->lx - (ThisTexture->bmWidth / 2);
	offset_y = obj->ly - (ThisTexture->bmHeight / 2);
	scale = obj->lz * 200;
	//debug1 = obj->lz;

	sprite_w = ThisTexture->bmWidth;
	sprite_h = ThisTexture->bmHeight;
	sprite_sw = sprite_w * scale;
	sprite_sh = sprite_h * scale;
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
	

	video_buffer = &video_bufferB[offset_y * words_per_line + (offset_x * 4)];
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
				MyTextureP = &MyTexturePP[int(scaleCnt) *4 ];
				if(MyTextureP[0]+MyTextureP[1]+MyTextureP[2])
				{
					pZBuffer[0] = qqqc;
					video_buffer[0] = MyTextureP[0] * objalpha + video_buffer[0] * objalphaInv;
					video_buffer[1] = MyTextureP[1] * objalpha + video_buffer[1] * objalphaInv;
					video_buffer[2] = MyTextureP[2] * objalpha + video_buffer[2] * objalphaInv;
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
					alpha = static_cast<float>(MyTextureP[3])*0.00392;
					alphaInv = 1 - alpha;
					video_buffer[0] = (video_buffer[0] * alphaInv) + (MyTextureP[0] * alpha);
					video_buffer[1] = (video_buffer[1] * alphaInv) + (MyTextureP[1] * alpha);
					video_buffer[2] = (video_buffer[2] * alphaInv) + (MyTextureP[2] * alpha);
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
				video_buffer[0] = MyTextureP[0];
				video_buffer[1] = MyTextureP[1];
				video_buffer[2] = MyTextureP[2];
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

}
//debug1 = timeGetTime()-debug1;
//debug1 = timeGetTime() - debug1;
//if(debug1) debug1 = debug1;
}



inline void RenderSprites3(void)
{
//debug1 = timeGetTime();

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
float scaleCnt = 0;
float qqqc = 0;
int video_rxz = 0;
float textureVpos = 0;
float textureVstep = 0;
int y = 0;
int x = 0;

for(int tmp=0;tmp<sprites_front_cnt;tmp++)
{
	obj = &sprites_front[tmp];

	if(!obj->visable) continue;
	obj->lz = 1/obj->lz;
	obj->lx = ((obj->lx) * obj->lz * TMS) + SCREEN_WIDTHh;
	obj->ly = ((obj->ly) * obj->lz * TMS) + SCREEN_HEIGHTh;
	if(obj->lx < 0 || obj->lx>SCREEN_WIDTH || obj->ly < 0 || obj->ly>SCREEN_HEIGHT) continue;
	if(obj->lz<0)continue;

	// Get pointer to this background objects texture
	cTexture *ThisTexture = obj->boTexture;
	//\scale = 2;
	// Get video offset of this sprite
	offset_x = obj->lx - (ThisTexture->bmWidth / 2);
	offset_y = obj->ly - (ThisTexture->bmHeight / 2);
	scale = obj->lz * 200;
	//debug1 = obj->lz;

	sprite_w = ThisTexture->bmWidth;
	sprite_h = ThisTexture->bmHeight;
	sprite_sw = sprite_w * scale;
	sprite_sh = sprite_h * scale;
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


	MyTextureP = ThisTexture->bmpBuffer;
	MyTexturePP = MyTextureP;

	scaleCnt = 0;
	qqqc = 0;
	video_rxz = video_rx / 4;	

	video_buffer = &video_bufferB[offset_y * words_per_line + (offset_x * 4)];
	pZBuffer = &ZBuffer[offset_y * SCREEN_WIDTH + offset_x];
	
	textureVpos = sprite_st / scale;
	textureVstep = (sprite_sh / scale - textureVpos) / (sprite_sh - sprite_st);;
	sprite_w = sprite_w << 2;

	for(y = sprite_st; y < sprite_sh; y++)
	{
		scaleCnt = scaleCnt_st;
		MyTexturePP = &ThisTexture->bmpBuffer[int(textureVpos) * sprite_w];

		for(x = sprite_sl; x < sprite_sw;x++)
		{
			MyTextureP = &MyTexturePP[int(scaleCnt) << 2];
			if(MyTextureP[2])
			{
			if((qqqc = obj->lz+ZBufferPage) > pZBuffer[0])
			{
				
				pZBuffer[0] = qqqc;
				video_buffer[2] = MyTextureP[2];
			}
			}
			video_buffer+=4;
			scaleCnt+=scaleStep;
			pZBuffer++;
		}

		textureVpos += textureVstep;
		video_buffer += video_rx;
		pZBuffer += video_rxz;
	}

}

//debug1 = timeGetTime()-debug1;
}





///// That stuff up above is all the direct draw and helper
/////	functions.. Now we get into the real stuff...
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////


// DATA STRUCTURES ////////////////////////////////////////////////////////////////////////////////

// FUNCTION PROTOTYPES ////////////////////////////////////////////////////////////////////////////
// GLOBALS ////////////////////////////////////////////////////////////////////////////////////////

// FUNCTIONS //////////////////////////////////////////////////////////////////////////////////////

inline void FixAngle(int &angle)
{
	if(angle>359) angle = angle - 360;
	if(angle<0) angle = angle + 360;
}

inline float MyCos(int &angle)
{
	if(angle>359) angle = angle - 360;
	if(angle<0) angle = angle + 360;
	return LocalCOS[angle];
}

inline float MySin(int &angle)
{
	if(angle>359) angle = angle - 360;
	if(angle<0) angle = angle + 360;
	return LocalSIN[angle];
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rotate the world poly set and lights about CamX,CamY,CamZ
//	return new world as Obt & LSt
//	perform basic culling.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void RotateWorld(void)
{	
	unsigned int a=0,b=0;
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
		zover = (1/curobj->wz) * TMS;
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
		tsc = curobj->scale * 40;
		if(curobj->wy - tsc < SCREEN_HEIGHT && curobj->wy + tsc  > 0 && curobj->wx  - tsc < SCREEN_WIDTH && curobj->wx + tsc > 0)
		{
			//debug3++;

			curobj->hidden = false;
			for(b=0;b<curobj->polycount;b++)
			{
				if(curobj->polygon[b].visable==0)
				{
					CalcNorm(curobj->polygon[b]);
					if(curobj->polygon[b].D<0) curobj->polygon[b].visable=int(45 * -curobj->polygon[b].D);
				} else curobj->polygon[b].visable--;

			}
		} else {
			curobj->hidden = true;
		}
	}

	//debug3 = WLD.obj[0].wy ;

}

inline void CalcNorm(polygon_type &Poly)
{

	// Calculate normal
	float dAx = (Poly.vertex[1]->wx - Poly.vertex[0]->wx);
	float dAy = (Poly.vertex[1]->wy - Poly.vertex[0]->wy);
	float dAz = (Poly.vertex[1]->wz - Poly.vertex[0]->wz);

	float dBx = (Poly.vertex[2]->wx - Poly.vertex[0]->wx);
	float dBy = (Poly.vertex[2]->wy - Poly.vertex[0]->wy);
	float dBz = (Poly.vertex[2]->wz - Poly.vertex[0]->wz);

	Poly.nX = (dAy * dBz - dAz * dBy);
	Poly.nY = (dAz * dBx - dAx * dBz);
	Poly.nZ = (dAx * dBy - dAy * dBx);

	//( A.y * B.z - A.z * B.y , A.z * B.x - A.x * B.z , A.x * B.y - A.y * B.x)

	//Or we can describe the same equation in terms of vector components:
	//A · B = A.x * B.x + A.y * B.y + A.z * B.z
	//Using a Dot Product, we can obtain the angle between two vectors A and B as follows:
	//Cos q = (A · B) / (|A| · |B|)

	//Poly.D = CosAngle(Poly.vertex[0]->wx+Poly.nX ,  Poly.vertex[0]->wy+Poly.nY, Poly.vertex[0]->wz+Poly.nZ,  Poly.vertex[0]->wx,  Poly.vertex[0]->wy, Poly.vertex[0]->wz);;
	Poly.D = CosAngle(Poly.nX ,  Poly.nY, Poly.nZ,  Poly.vertex[0]->wx,  Poly.vertex[0]->wy, Poly.vertex[0]->wz);;
	//Poly.D = 1;
}


inline float CosAngle(float &AX, float &AY, float &AZ, float &bx, float &by, float &bz)
{
	return float((AX * bx + AY * by + AZ * bz) / (sqrt(AX * AX + AY * AY + AZ * AZ) * sqrt(bx * bx + by * by + bz * bz)));
}



inline void MeshLights()
{
	//
	//	Load and set up App Objects and Lights
	//

	char buffer[50];
	pclip.vertcount = 0;
	LX xloader;

	xloader.lx_hwnd = g_hwnd;
	xloader.GlobalScale = 1;
	xloader.LoadFile("c:\\Temp\\zspace\\fighter.X");
	
	for(int i=0;i<mines_cnt;i++)
		xloader.LoadFile("c:\\Temp\\zspace\\mine.X");

	//xloader.LoadFile("c:\\Temp\\zspace\\earth.X");

	xloader.CleanUp();

	// number of objects in scene
	WLD.objcount=xloader.GetObjectCount();
	// Alocate memory for objects.
	WLD.obj = new object_type[WLD.objcount];
	//MessageBox ( g_hwnd, "Everything is ok", "Hello...", MB_OK | MB_ICONSTOP );
	long vertCnt=0;
	long faceCnt=0;

	for (i=0; i<xloader.GetObjectCount(); i++)
	{
		object_type *curobj = &WLD.obj[i];						// assogn pointer CUROBJ to current object

		curobj->thrust = 0;
		curobj->vx = 0;
		curobj->vy = 0;//;((rand()%10)-20)/10;
		curobj->vz = 0;
		curobj->an_X = 0;
		curobj->an_Y = 0;
		curobj->an_Z = 0;
		curobj->x = xloader.GetObject(i).x;
		curobj->y = -xloader.GetObject(i).z;
		curobj->z = xloader.GetObject(i).y;

		//sprintf(buffer, "x=%f y=%f z=%f", curobj->x,curobj->y,curobj->z); 
		//MessageBox	( g_hwnd, buffer, "Rendering...", MB_OK | MB_ICONSTOP );


		//curobj->vertcount = xloader.GetObject(i).GetVertexCount();	// Number of verts in this object?
		curobj->vertcount = xloader.GetObject(i).GetVertexCount();	// Number of verts in this object?
		curobj->vertex = new vertex_type[curobj->vertcount];	// Allocate memory for vertex array;

		for(int v=0; v<curobj->vertcount; v++)
		{
			vertex_type *curvert=&curobj->vertex[v];
			curvert->lx = xloader.GetObject(i).GetVertex(v).lx;
			curvert->ly = -xloader.GetObject(i).GetVertex(v).lz;
			curvert->lz = xloader.GetObject(i).GetVertex(v).ly;
			vertCnt++;
		}

		curobj->polycount = xloader.GetObject(i).GetPolygonCount();	// Set number of polygons in object
		
		curobj->polygon = new polygon_type[curobj->polycount];	// Assign memory for polygo array
		for(int p=0; p<curobj->polycount; p++)
		{
			polygon_type *curpoly = &curobj->polygon[p];

			// ********** NOT needed now since vertex count is always = 3
			//curpoly->vertcount = 3;
			// Assign memory for vertex array
			//curpoly->vertex = new vertex_type*[curpoly->vertcount];
			//curpoly->uv = new uv_type[curpoly->vertcount];
			// Create an array of pointers to verticies.
			// Allocate memory for pointer and point it at vertex in vertex array;
			curpoly->vertex[2] = &curobj->vertex[xloader.GetObject(i).GetPolygon(p).vertex[0]];
			curpoly->uv[2].u = xloader.GetObject(i).GetPolygon(p).uv[0].u;
			curpoly->uv[2].v = xloader.GetObject(i).GetPolygon(p).uv[0].v;

			curpoly->vertex[1] = &curobj->vertex[xloader.GetObject(i).GetPolygon(p).vertex[1]];
			curpoly->uv[1].u = xloader.GetObject(i).GetPolygon(p).uv[1].u;
			curpoly->uv[1].v = xloader.GetObject(i).GetPolygon(p).uv[1].v;

			curpoly->vertex[0] = &curobj->vertex[xloader.GetObject(i).GetPolygon(p).vertex[2]];
			curpoly->uv[0].u = xloader.GetObject(i).GetPolygon(p).uv[2].u;
			curpoly->uv[0].v = xloader.GetObject(i).GetPolygon(p).uv[2].v;

			curpoly->SurfaceTexture = xloader.GetObject(i).GetPolygon(p).SurfaceTexture;
			curpoly->visable=0;
			faceCnt++;
		}

	}

	
	TextureCnt = xloader.GetTextureCount();
	int tmpmapcnt;
	for(i=0; i<TextureCnt;i++)
	{
		bool loadmap = true;

		Texture[i].faceR = xloader.GetTexture(i).faceR;
		Texture[i].faceG = xloader.GetTexture(i).faceG;
		Texture[i].faceB = xloader.GetTexture(i).faceB;
		
		if(strlen(xloader.GetTexture(i).bmFileName)>5)
		{
			//sprintf(buffer, "%s", xloader.GetTexture(i).bmFileName); 
			//MessageBox	( g_hwnd, buffer, "longer...", MB_OK | MB_ICONSTOP );

			for(int p=0; p<i; p++)
			//for(int p=0; p<TextureCnt; p++)
			{
				if(p!=i)
				{
					if(lstrcmpi(xloader.GetTexture(p).bmFileName, xloader.GetTexture(i).bmFileName)==0)
					//if(strcmp(xloader.GetTexture(i).bmFileName, Texture[p].filename))
					{
						for(int ob=0;ob<WLD.objcount;ob++)
							for(int v=0;v<WLD.obj[ob].polycount;v++)
								if(WLD.obj[ob].polygon[v].SurfaceTexture==i)
								{
									WLD.obj[ob].polygon[v].SurfaceTexture = p;
								}


						//sprintf(buffer, "%s %i",xloader.GetTexture(i).bmFileName,p); 
						//MessageBox	( g_hwnd, buffer, "Rendering...", MB_OK | MB_ICONSTOP );

						loadmap = false;
						break;
						Texture[i].bmpBuffer = Texture[p].bmpBuffer;
					}
				}
			}

			if(loadmap)
			{\
				LoadTexture(Texture[i], xloader.GetTexture(i).bmFileName,false);
				tmpmapcnt++;
				//MessageBox	( g_hwnd, "Loaded Texture Map", "...", MB_OK | MB_ICONSTOP );
			}
		}

	}
	
	//TextureCnt = tmpmapcnt;
	sprintf(buffer, "%d textures %d maps  %d meshes, %d faces, %d verts\n",TextureCnt,tmpmapcnt, WLD.objcount,faceCnt,vertCnt); 
	//sprintf(buffer, "%s%s\n","test.bmp","_alpha");
	MessageBox	( g_hwnd, buffer, "Rendering...", MB_OK | MB_ICONSTOP );
	//PostMessage ( g_hwnd, WM_DESTROY, 0, 0 );
	//for (int i=0; i<a; i++)
	//{
	//	sprintf(buffer, "Mesh %d has %d vertices and %d triangles\n", i, loader.GetMesh(i).GetVertexCount(), loader.GetMesh(i).GetTriangleCount()  );
	//	MessageBox	( g_hwnd, buffer, "Polys", MB_OK | MB_ICONSTOP );
	//}

	xloader.Clear();
}



void SetUpWorld(void)
{

	// Calculate Sin/Cos Tables
	for(int tmp=0;tmp<360;tmp++)
	{
		LocalSIN[tmp]=float(sin(tmp*Rad));
		LocalCOS[tmp]=float(cos(tmp*Rad));
	}
	
	TextureCnt=0;
	MeshLights();
	
	// Set up back ground objects
	//LoadTexture(Texture[TextureCnt++], "data\\platform.bmp",false);
	
	LoadTexture(Texture[TextureCnt++], "data\\star2.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\earth.bmp",false);
	LoadTextureAlpha(Texture[TextureCnt-1], "data\\earth_alpha.bmp",false);
	
	sprites_back_cnt = 10;//500;
	sprites_back = new sprite[sprites_back_cnt];
	
	int an = 0;
	int anb = 0;
	for(tmp=0;tmp < sprites_back_cnt;tmp++)
	{
		an = rand()%359;
		anb=  rand()%359;
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

	sprites_front_cnt=2000;
	sprites_front = new sprite[sprites_front_cnt];	

	LoadTexture(Texture[TextureCnt++], "data\\shot_00.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\shot_01.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\shot_02.bmp",false);
	for(tmp=0;tmp < sprites_front_cnt;tmp++)
	{
		sprites_front[tmp].x =  0;
		sprites_front[tmp].z =  tmp;
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
		sprites_front[tmp].animation_len = 1;
		sprites_front[tmp].animation_len = TextureCnt-1;
		sprites_front[tmp].boTexture = &Texture[TextureCnt-1];
		sprites_front[tmp].alpha = 0.5;
	}
	ShotSprite = TextureCnt-3;
	ShotSprite_len = 3;
	LoadTexture(Texture[TextureCnt++], "data\\shrap.bmp",false);
	ShrapSprite = TextureCnt-1;
	ShrapSprite_len = 1;

	// Set up forground sprites

	char tbuffer[32];
	for(tmp=0 ; tmp<62 ; tmp+=2)
	{
		sprintf(tbuffer, "data\\explosion%.4d.bmp",tmp); 
		LoadTexture(Texture[TextureCnt++],tbuffer,false);
		sprintf(tbuffer, "data\\explosion_Alpha%.4d.bmp",tmp); 
		LoadTextureAlpha(Texture[TextureCnt-1],tbuffer,false);

	}
	MessageBox	( g_hwnd,tbuffer, "Rendering...", MB_OK | MB_ICONSTOP );
	/*
	LoadTexture(Texture[TextureCnt++], "data\\explosion0000.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0002.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0004.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0006.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0008.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0010.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0012.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0014.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0016.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0018.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0020.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0022.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0024.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0026.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0028.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0030.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0032.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0034.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0036.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0038.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0040.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0042.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0044.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0046.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0048.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0050.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0052.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0054.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0056.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0058.bmp",false);
	LoadTexture(Texture[TextureCnt++], "data\\explosion0060.bmp",false);
	*/
	BoomSprite = TextureCnt-32;
	BoomSprite_len = 32;

	// Set up astrds
	int ststart = 1;
	//int an;
	for(tmp=0;tmp<mines_cnt;tmp++)
	{
		//WLD.obj[tmp+ststart].x = rand()%1500 - 750;
		//WLD.obj[tmp+ststart].z = rand()%750-100;
		WLD.obj[tmp+ststart].x = rand()%3000 - 1500;
		WLD.obj[tmp+ststart].z = rand()%3000-1500;
		WLD.obj[tmp+ststart].y = 0;//rand()%1500 - 750;

		an = rand()%360-180;
		WLD.obj[tmp+ststart].vax = ((rand()%10)-5)/4;//((MyCos(an) * 10)-5)/4;
		WLD.obj[tmp+ststart].vay = 0;
		WLD.obj[tmp+ststart].vaz = ((rand()%10)-5)/4;//((MyCos(an) * 10)-5)/4;
		an = rand()%359;
		anb = rand()%20;
		WLD.obj[tmp+ststart].vx =  (MyCos(an)/10 * anb);
		WLD.obj[tmp+ststart].vy =  0; //(rand()%2 - 1);
		WLD.obj[tmp+ststart].vz =  (MySin(an)/10 * anb);

		WLD.obj[tmp+ststart].scale = rand()%10+1;
	}



	CamX = 0; CamY = -100; CamZ = 0;
	CamOb = 0;
	camobj = &WLD.obj[CamOb];
	camobj->x = 0;
	camobj->y = 0;
	camobj->z = 0;
	camobj->scale = 1;

	an_X = -90;
	AltRender = false;
	AltSprite = false;

	RotateWorld();
}


void DestroyWorld(void)
{
	delete [] sprites_back;
	delete [] sprites_front;

	for(int a=0;a<TextureCnt;a++)
	{
		delete [] Texture[a].bmpBuffer;
	}
	delete [] Texture;
	
	for(a=0;a<WLD.objcount;a++)
	{
		// Why o Why? does it crash...
		//for(int b=0; a<WLD.obj[a].polycount;b++)
		//{
		//	delete [] WLD.obj[a].polygon[b].uv;
		//}
		
		delete [] WLD.obj[a].polygon;
		delete [] WLD.obj[a].vertex;
	
	}
	delete [] WLD.obj;
	delete [] ZBuffer;
	//MessageBox	( g_hwnd, "World Destroyed", "Hello...", MB_OK | MB_ICONSTOP );
}


inline void MatMult(float mat1[4][4], float mat2[4][4])
{
	// MatMult.. What follows is the unroaled version of this.
	
    float temp[4][4];
	
    for(short unsigned int i = 0; i<4; i++)
	{
        //for(short unsigned int j = 0; j<4; j++)
			//temp[i][j] = (mat1[i][j] * mat2[0][j]) + (mat1[i][1] * mat2[1][j]) + (mat1[i][2] * mat2[2][j]) + (mat1[i][3] * mat2[3][j]);
            temp[i][0] = (mat1[i][0] * mat2[0][0]) + (mat1[i][1] * mat2[1][0]) + (mat1[i][2] * mat2[2][0]) + (mat1[i][3] * mat2[3][0]);
		    temp[i][1] = (mat1[i][0] * mat2[0][1]) + (mat1[i][1] * mat2[1][1]) + (mat1[i][2] * mat2[2][1]) + (mat1[i][3] * mat2[3][1]);
            temp[i][2] = (mat1[i][0] * mat2[0][2]) + (mat1[i][1] * mat2[1][2]) + (mat1[i][2] * mat2[2][2]) + (mat1[i][3] * mat2[3][2]);
            temp[i][3] = (mat1[i][0] * mat2[0][3]) + (mat1[i][1] * mat2[1][3]) + (mat1[i][2] * mat2[2][3]) + (mat1[i][3] * mat2[3][3]);
	}

	for(i=0;i<4;i++)
	{
		mat1[i][0] = temp[i][0];
		mat1[i][1] = temp[i][1];
		mat1[i][2] = temp[i][2];
		mat1[i][3] = temp[i][3];
	}

}

inline void RotatePntExB(float Matrix[4][4], int AngX, int AngY, int AngZ)
{   
    float rmat[4][4];

	if(AngX>359) AngX = AngX - 360;
	if(AngX<0) AngX = AngX + 360;
	if(AngY>359) AngY = AngY - 360;
	if(AngY<0) AngY = AngY + 360;
	if(AngZ>359) AngZ = AngZ - 360;
	if(AngZ<0) AngZ = AngZ + 360;
/*
    rmat[0][0] = LocalCOS[AngY]*LocalCOS[AngZ]; rmat[0][1] = LocalSIN[AngZ]					;rmat[0][2] = -LocalSIN[AngY];					rmat[0][3] = 0;
    rmat[1][0] = -LocalSIN[AngZ];               rmat[1][1] = LocalCOS[AngZ]*LocalCOS[AngX]	;rmat[1][2] = LocalSIN[AngX];					rmat[1][3] = 0;
    rmat[2][0] = LocalSIN[AngY];                rmat[2][1] = -LocalSIN[AngX]				;rmat[2][2] = LocalCOS[AngY]*LocalCOS[AngX];	rmat[2][3] = 0;
    rmat[3][0] = 0;								rmat[3][1] = 0								;rmat[3][2] = 0;								rmat[3][3] = 1;
    MatMult(Matrix, rmat);
*/

    // X
    rmat[0][0] = 1;               rmat[0][1] = 0				;rmat[0][2] = 0;               rmat[0][3] = 0;
    rmat[1][0] = 0;               rmat[1][1] = LocalCOS[AngX]	;rmat[1][2] = LocalSIN[AngX]; rmat[1][3] = 0;
    rmat[2][0] = 0;               rmat[2][1] = -LocalSIN[AngX]	;rmat[2][2] = LocalCOS[AngX]; rmat[2][3] = 0;
    rmat[3][0] = 0;               rmat[3][1] = 0				;rmat[3][2] = 0;               rmat[3][3] = 1;
    MatMult(Matrix, rmat);

	// Z
    rmat[0][0] = LocalCOS[AngZ]	; rmat[0][1] = LocalSIN[AngZ]		; rmat[0][2] = 0; rmat[0][3] = 0;
    rmat[1][0] = -LocalSIN[AngZ]; rmat[1][1] = LocalCOS[AngZ]		; rmat[1][2] = 0; rmat[1][3] = 0;
    rmat[2][0] = 0				; rmat[2][1] = 0					; rmat[2][2] = 1; rmat[2][3] = 0;
    rmat[3][0] = 0				; rmat[3][1] = 0					; rmat[3][2] = 0; rmat[3][3] = 1;
    MatMult(Matrix, rmat);

    // Y
    rmat[0][0] = LocalCOS[AngY]; rmat[0][1] = 0	; rmat[0][2] = -LocalSIN[AngY]	; rmat[0][3] = 0;
    rmat[1][0] = 0				; rmat[1][1] = 1; rmat[1][2] = 0				; rmat[1][3] = 0;
    rmat[2][0] = LocalSIN[AngY]; rmat[2][1] = 0	; rmat[2][2] = LocalCOS[AngY]	; rmat[2][3] = 0;
    rmat[3][0] = 0				; rmat[3][1] = 0; rmat[3][2] = 0				; rmat[3][3] = 1;
    MatMult(Matrix, rmat);

}

inline void Matrix_Ident(float Matrix[4][4])
{
    Matrix[0][0] = 1; Matrix[0][1] = 0; Matrix[0][2] = 0; Matrix[0][3] = 0;
    Matrix[1][0] = 0; Matrix[1][1] = 1; Matrix[1][2] = 0; Matrix[1][3] = 0;
    Matrix[2][0] = 0; Matrix[2][1] = 0; Matrix[2][2] = 1; Matrix[2][3] = 0;
    Matrix[3][0] = 0; Matrix[3][1] = 0; Matrix[3][2] = 0; Matrix[3][3] = 1;
}

inline void RotateAxis(float Matrix[4][4], int Ang, float u, float v, float w)
{   
    float rmat[4][4];

	if(Ang>359) Ang = Ang - 360;
	if(Ang<0) Ang = Ang + 360;

    rmat[0][0] = LocalCOS[Ang]  + (u*u)*(1-LocalCOS[Ang]); rmat[0][1] = -w*LocalSIN[Ang]+u*v*(1-LocalCOS[Ang]);	rmat[0][2] = v*LocalSIN[Ang]+u*w*(1-LocalCOS[Ang]);					rmat[0][3] = 0;
    rmat[1][0] = w*LocalSIN[Ang]+ u*v*(1-LocalCOS[Ang]); rmat[1][1] = LocalCOS[Ang]+(v*v)*(1-LocalCOS[Ang]);		rmat[1][2] = -u*LocalSIN[Ang]+v*w*(1-LocalCOS[Ang]);					rmat[1][3] = 0;
    rmat[2][0] = -v*LocalSIN[Ang]+u*w*(1-LocalCOS[Ang]); rmat[2][1] = u*LocalSIN[Ang]+v*w*(1-LocalCOS[Ang])	;		rmat[2][2] = LocalCOS[Ang]+(w*w)*(1-LocalCOS[Ang]);	rmat[2][3] = 0;
    rmat[3][0] = 0;								rmat[3][1] = 0								;rmat[3][2] = 0;								rmat[3][3] = 1;
    MatMult(Matrix, rmat);
}

	

inline void RotatePntExBe(float Matrix[4][4], int AngX, int AngY, int AngZ)
{   
    float rmat[4][4];

	if(AngX>359) AngX = AngX - 360;
	if(AngX<0) AngX = AngX + 360;
	if(AngY>359) AngY = AngY - 360;
	if(AngY<0) AngY = AngY + 360;
	if(AngZ>359) AngZ = AngZ - 360;
	if(AngZ<0) AngZ = AngZ + 360;


    rmat[0][0] = LocalCOS[AngY]*LocalCOS[AngZ]; rmat[0][1] = LocalSIN[AngZ]					;rmat[0][2] =  -LocalSIN[AngY];               rmat[0][3] = 0;
    rmat[1][0] = -LocalSIN[AngZ];               rmat[1][1] = LocalCOS[AngZ]*LocalCOS[AngX]	;rmat[1][2] = LocalSIN[AngX]; rmat[1][3] = 0;
    rmat[2][0] = LocalSIN[AngY];                rmat[2][1] = -LocalSIN[AngX]				;rmat[2][2] = LocalCOS[AngY]*LocalCOS[AngX]; rmat[2][3] = 0;
    rmat[3][0] = 0;								rmat[3][1] = 0								;rmat[3][2] = 0;               rmat[3][3] = 1;

    MatMult(Matrix, rmat);

    /*
	// X
    rmat[0][0] = 1;               rmat[0][1] = 0			;rmat[0][2] = 0;               rmat[0][3] = 0;
    rmat[1][0] = 0;               rmat[1][1] = LocalCOS[AngX]	;rmat[1][2] = LocalSIN[AngX]; rmat[1][3] = 0;
    rmat[2][0] = 0;               rmat[2][1] = -LocalSIN[AngX]	;rmat[2][2] = LocalCOS[AngX]; rmat[2][3] = 0;
    rmat[3][0] = 0;               rmat[3][1] = 0			;rmat[3][2] = 0;               rmat[3][3] = 1;
    MatMult(Matrix, rmat);
    // Y
    rmat[0][0] = LocalCOS[AngY]; rmat[0][1] = 0; rmat[0][2] = -LocalSIN[AngY]	; rmat[0][3] = 0;
    rmat[1][0] = 0				; rmat[1][1] = 1; rmat[1][2] = 0				; rmat[1][3] = 0;
    rmat[2][0] = LocalSIN[AngY]; rmat[2][1] = 0; rmat[2][2] = LocalCOS[AngY]	; rmat[2][3] = 0;
    rmat[3][0] = 0				; rmat[3][1] = 0; rmat[3][2] = 0				; rmat[3][3] = 1;
    MatMult(Matrix, rmat);

	    // Z
    rmat[0][0] = LocalCOS[AngZ]	; rmat[0][1] = LocalSIN[AngZ]	; rmat[0][2] = 0; rmat[0][3] = 0;
    rmat[1][0] = -LocalSIN[AngZ]	; rmat[1][1] = LocalCOS[AngZ]	; rmat[1][2] = 0; rmat[1][3] = 0;
    rmat[2][0] = 0				; rmat[2][1] = 0			; rmat[2][2] = 1; rmat[2][3] = 0;
    rmat[3][0] = 0				; rmat[3][1] = 0			; rmat[3][2] = 0; rmat[3][3] = 1;
    MatMult(Matrix, rmat);
*/
}


inline void Translate(float Matrix[4][4], float &Xt, float &Yt, float &Zt)
{   
    float tmat[4][4];

    tmat[0][0] = 1	; tmat[0][1] = 0 ; tmat[0][2] = 0; tmat[0][3] = 0;
    tmat[1][0] = 0	; tmat[1][1] = 1 ; tmat[1][2] = 0; tmat[1][3] = 0;
    tmat[2][0] = 0	; tmat[2][1] = 0 ; tmat[2][2] = 1; tmat[2][3] = 0;
    tmat[3][0] = Xt	; tmat[3][1] = Yt; tmat[3][2] = Zt; tmat[3][3] = 1;
    MatMult(Matrix, tmat);
}

inline void Transform(float Matrix[4][4], float &pX, float &pY, float &pZ)
{
    float outX = pX * Matrix[0][0] + pY * Matrix[1][0] + pZ * Matrix[2][0] + Matrix[3][0];
    float outY = pX * Matrix[0][1] + pY * Matrix[1][1] + pZ * Matrix[2][1] + Matrix[3][1];
    float outZ = pX * Matrix[0][2] + pY * Matrix[1][2] + pZ * Matrix[2][2] + Matrix[3][2];
    pX = outX; pY = outY; pZ = outZ;
}



unsigned int polyx[SCREEN_HEIGHT][2];
float polyz[SCREEN_HEIGHT][2];
float polyuv[SCREEN_HEIGHT][4];




void zclip(polygon_type *polygon,cliped_polygon_type *clip)
{
	float t;
	clip_type *pcv=clip->vertex;

	int cp=0;
	int zmin=2;

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
	int ymin=1;
	int ymax=SCREEN_HEIGHT-1;
	int xmin=1;
	int xmax=SCREEN_WIDTH-1;

	// Clip to screen top at ymin
	int cp=0;
	int v1=clip->vertcount-1;

	for(int v2=0; v2<clip->vertcount; v2++)
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
	for(v2=0; v2<clip->vertcount; v2++)
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

	for(v2=0; v2<clip->vertcount; v2++)
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
	for(v2=0; v2<clip->vertcount; v2++)
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


inline void RenderW()
{
	//return;
	//debug2 = timeGetTime();
	
	//debug2 = 0;

	int i;

	float zover;
	float qqqc;

	// Reasterizer vars
	int a=0;

	// Daves Rasterizer.
	int vertCnt;
	int TopVr,BotVr,TopYr,BotYr;
	int TopVl,BotVl,TopYl,BotYl;
	float DistanceR;
	float DistanceL;
	float tVal	;
	float StepZ;
	float StepZL,SlpL;
	float StepZR,SlpR;
	float Xl,Xr;
	float Zl,Zr;

	float tZ;
	float DeltaX;

	int t;

	int poly_cnt;
	int obj_cnt;
	object_type *curobj;
	polygon_type *curpolygon;


	// For every object in the World
	for(obj_cnt=0;obj_cnt<WLD.objcount;obj_cnt++)
	{
		curobj = &WLD.obj[obj_cnt];	// Pointer to this object.
		if(curobj->hidden) continue;
		if(curobj->scale==0) continue;
		// For every polygon in an object
		for(poly_cnt=0;poly_cnt<curobj->polycount;poly_cnt++)
		{
			curpolygon = &curobj->polygon[poly_cnt];// Pointer to this polygon.

			if(curpolygon->visable>0) continue;	// Calculated in rotate world. >0 indicates not visable.


			zclip(curpolygon,&pclip);		// Clip polygon to frontZ and return pclip
			if(pclip.vertcount<3) continue;	// No points left in polygon so move on to next


			for(t=0; t<pclip.vertcount; t++) {  // Project verts to screen space.
				zover = 1/pclip.vertex[t].z;	// zclip will ensure z is never 0
				pclip.vertex[t].x1 = (pclip.vertex[t].x * TMS * zover)+SCREEN_WIDTHh;
				pclip.vertex[t].y1 = (pclip.vertex[t].y * TMS * zover)+SCREEN_HEIGHTh;
				pclip.vertex[t].z1 = zover;
			}
			
			tclip(&pclip);	// Clip polygon against screen

			if(pclip.vertcount<3) continue;	// No points left in polygon so move on to next

			byte faceshade = curpolygon->D * 255;
			
			// Rasterize the dave way..
			TopVr = 0 ; TopYr = pclip.vertex[0].y1;
			vertCnt = pclip.vertcount;
			for(a = 0; a<vertCnt; a++)
			{
				if(pclip.vertex[a].y1 < TopYr)
				{
					TopVr = a; TopYr = pclip.vertex[a].y1;
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
					if(!vertCnt) break;
					vertCnt--;
					TopVr++;
					if(TopVr == pclip.vertcount) TopVr = 0;
					if(TopVr == pclip.vertcount-1) BotVr = 0; else BotVr = TopVr + 1;

					TopYr = pclip.vertex[TopVr].y1;
					BotYr = pclip.vertex[BotVr].y1;
					DistanceR = TopYr - BotYr;
					if(!DistanceR) goto NewR;

					Xr = pclip.vertex[TopVr].x1;
					Zr = pclip.vertex[TopVr].z1;

					tVal = 1 / DistanceR;
					SlpR = (Xr - pclip.vertex[BotVr].x1) * tVal;
					StepZR = (Zr - pclip.vertex[BotVr].z1) * tVal;					
				}


				if(!DistanceL)
				{
				newL:
					if(!vertCnt) break;
					vertCnt--;
					TopVl--;
					if(TopVl==-1) TopVl = pclip.vertcount - 1;
					if(TopVl==0) BotVl = pclip.vertcount - 1; else BotVl = TopVl - 1;
            
					TopYl = pclip.vertex[TopVl].y1; BotYl = pclip.vertex[BotVl].y1;
					DistanceL = TopYl - BotYl;
					if(!DistanceL) goto newL;
					tVal = 1 / DistanceL;

					Xl = pclip.vertex[TopVl].x1+1;
					Zl = pclip.vertex[TopVl].z1;

					SlpL = (Xl - pclip.vertex[BotVl].x1) * tVal;
					StepZL = (Zl - pclip.vertex[BotVl].z1) * tVal;
				}
        

				//////////////////////////////////////
		        DeltaX = Xr - Xl+1;
				if(DeltaX)
				{
					DeltaX = 1/DeltaX;				
					StepZ = ((Zr - Zl) * DeltaX);

					tZ = Zl;

					video_buffer = video_bufferB+(int(Xl)*4) + (TopYr * words_per_line);
					pZBuffer = &ZBuffer[int(Xl) + (TopYr*SCREEN_WIDTH)];	

					for(i = Xl; i<Xr;i++)
					{
						if((qqqc = tZ+ZBufferPage) > pZBuffer[0])
						{
							pZBuffer[0] = qqqc;						
							//memset(video_buffer,(curpolygon->D)*255,3);
							//video_buffer[0] = faceshade;
							video_buffer[1] = faceshade;
							//video_buffer[2] = faceshade;
						}

						tZ+=StepZ;

						video_buffer+=4;
						pZBuffer++;
					}
				}
       
				//////////////////////////////////////
				Xr+=SlpR;
				Zr+=StepZR;

				Xl+=SlpL;
				Zl+=StepZL;
        
				DistanceR++;
				DistanceL++;
				TopYr++;
				TopYl++;
			}

		}

		
	}
     
	//debug2 = timeGetTime()-debug2;

}


inline void RenderTF()
{
	//debug2 = timeGetTime()-debug2;
	
	//debug2 = 0;
	
	//ccc++;
	//if(ccc==256) ccc=0;
	float SurfaceMultW;
	float SurfaceMultH;


	BYTE * dibits;

	BYTE x,y;
	int i;

	float zover;
	float qqqc;

	bool hasmap;
	//byte faceR,faceG,faceB;
	byte faceC[4];

	cTexture *MyTexture;// = Texture[1];
	BYTE *MyTextureP;
	// Reasterizer vars
	int a=0;

	// Daves Rasterizer.
	unsigned int vertCnt;
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

	int t;

	int poly_cnt;
	int obj_cnt;
	object_type *curobj;
	polygon_type *curpolygon;


	// Clear ZBuffer if needed 

	//BYTE*					video_bufferC		= NULL;	// used to draw onto back-buffer
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
		for(poly_cnt=0;poly_cnt<curobj->polycount;poly_cnt++)
		{
			curpolygon = &curobj->polygon[poly_cnt];// Pointer to this polygon.

			if(curpolygon->visable>0) continue;	// Calculated in rotate world. >0 indicates not visable.

			zclip(curpolygon,&pclip);		// Clip polygon to frontZ and return pclip
			if(pclip.vertcount<3) continue;	// No points left in polygon so move on to next


			for(t=0; t<pclip.vertcount; t++) {  // Project verts to screen space.
				zover = 1/pclip.vertex[t].z;	// zclip will ensure z is never 0
				pclip.vertex[t].x1 = (pclip.vertex[t].x * TMS * zover) + SCREEN_WIDTHh;
				pclip.vertex[t].y1 = (pclip.vertex[t].y * TMS * zover) + SCREEN_HEIGHTh;
				pclip.vertex[t].z1 = zover;
				pclip.vertex[t].u1 = pclip.vertex[t].u * zover;	// uv texture cords.
				pclip.vertex[t].v1 = pclip.vertex[t].v * zover;

			}
			
			tclip(&pclip);	// Clip polygon against screen

			if(pclip.vertcount<3) continue;	// No points left in polygon so move on to next

			// Get Texture details

			MyTexture = &Texture[curpolygon->SurfaceTexture];
			
			if(MyTexture->bmWidth && !AltRender)
			{
				hasmap = true;
				MyTextureP   = MyTexture->bmpBuffer;
				SurfaceMultH = MyTexture->bmHeight;
				SurfaceMultW = MyTexture->bmWidth;
			} else {
				hasmap = false;
				faceC[0] = MyTexture->faceB * curpolygon->D;
				faceC[1] = MyTexture->faceG * curpolygon->D;
				faceC[2] = MyTexture->faceR * curpolygon->D;
				faceC[3] = NULL;
			}

			
			////////////////////////// Rasterize the dave way..
			TopVr = 0 ; TopYr = pclip.vertex[0].y1;
			vertCnt = pclip.vertcount;
			// Find top vertex
			for(a = 0; a<vertCnt; a++)
			{
				if(pclip.vertex[a].y1 < TopYr)
				{
					TopVr = a; TopYr = pclip.vertex[a].y1;
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

					TopYr = pclip.vertex[TopVr].y1;
					BotYr = pclip.vertex[BotVr].y1;

					DistanceR = TopYr - BotYr;
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
            
					TopYl = pclip.vertex[TopVl].y1;
					BotYl = pclip.vertex[BotVl].y1;

					DistanceL = TopYl - BotYl;
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

					video_buffer = video_bufferB+(int(Xl)*4) + (TopYr * words_per_line);
					pZBuffer = ZBuffer + int(Xl) + (TopYr*SCREEN_WIDTH);	

					int res =15;

					if(hasmap)
					{

						StepU = (Ur - Ul) * DeltaX * SurfaceMultW;
						StepV = (Vr - Vl) * DeltaX * SurfaceMultH;

						tU = Ul*SurfaceMultW;
						tV = Vl*SurfaceMultH;

						for(i = Xr; i>Xl-1;--i)
						// = for(i = Xl; i<Xr;i++)
						{
							if((qqqc = tZ+ZBufferPage) > *pZBuffer)
							{
								*pZBuffer = qqqc;						
								if(res==15)
								{
									res=0;
									OneOverZ = 1 / tZ;
								} else res++;

								x = tU * OneOverZ;
								y = tV * OneOverZ;
								dibits = &MyTextureP[(x*4) + (y*1024)];
								video_buffer[0] = dibits[0];
								video_buffer[1] = dibits[1];
								video_buffer[2] = dibits[2];
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
								video_buffer[0] = faceC[0];
								video_buffer[1] = faceC[1];
								video_buffer[2] = faceC[2];
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

		}

		
	}
     
	//debug2 = timeGetTime()-debug2;
}






bool LoadTexture(cTexture &MyTexture, LPTSTR szFileName, bool force256)
{

	// Loads 24 bit Bitmap from file and streches it to fit our
	// standard 256x256 texture memory map. 

	BITMAP        bm;
	HBITMAP       hBitmap;
	HPALETTE      hPalette;
	BYTE		*tmpBMP;

	if( LoadBitmapFromBMPFile( szFileName, &hBitmap, &hPalette ) )
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
		//MyTexture.filename = szFileName;
		sprintf(MyTexture.filename, szFileName); 
		// Create temp and buffer btye arrays for Bitmap data.
		tmpBMP = new BYTE[bm.bmWidth * bm.bmHeight * 4];		// Source size.
		// This is our texture byte array. Why 4 * 256 * 256 ? Well even though.
		// we only need 3 bytes per pixel for RGB it is much faster to get bits
		// out of out texture map this way.. consider the following if we used 3 bytes
		// and any texture Width..
		//		dibits += (x * 3) + (y * TextureWidth * 3);
		// You can calculate that every poly pixel or this faster way.
		//		dibits += (x<<2) + (y<<10);

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
		
		GetBitmapBits(hBitmap,bm.bmWidthBytes*bm.bmHeight,tmpBMP);

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
		delete(tmpBMP);

		MyTexture.hasAlpha = false;

		return true;
	} else {
		return false;
	}
}



bool LoadTextureAlpha(cTexture &MyTexture, LPTSTR szFileName, bool force256)
{

	// Loads 24 bit Bitmap from file and streches it to fit our
	// standard 256x256 texture memory map. 
	
	BITMAP        bm;
	HBITMAP       hBitmap;
	HPALETTE      hPalette;
	BYTE		*tmpBMP;
	
	if( LoadBitmapFromBMPFile( szFileName, &hBitmap, &hPalette ) )
	{

		GetObject( hBitmap, sizeof(BITMAP), &bm );

		// Create temp and buffer btye arrays for Bitmap data.
		tmpBMP = new BYTE[bm.bmWidth * bm.bmHeight * 4];		// Source size.


		//MyTexture.bmpBuffer= new BYTE[4 * MyTexture.bmWidth * MyTexture.bmHeight];
		GetBitmapBits(hBitmap,bm.bmWidthBytes*bm.bmHeight,tmpBMP);

		// Converting bitmap to continous byte array.. Saves doing the same
		// sort of math per pixel later on Also we are streching the origional
		// Bitmap to fit 256x256.
		//MessageBox	( g_hwnd, szFileName, "Rendering...", MB_OK | MB_ICONSTOP );
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
		delete(tmpBMP);

		MyTexture.hasAlpha = true;

		return true;
	} else {
		return false;
	}
}



bool LoadBitmapFromBMPFile( LPTSTR szFileName, HBITMAP *phBitmap,HPALETTE *phPalette )
{
   BITMAP  bm;

   *phBitmap = NULL;
   *phPalette = NULL;

   //Public Declare Function LoadBitmap Lib "user32" Alias "LoadBitmapA" (ByVal hInstance As Long, ByVal lpBitmapName As String) As Long
	
   // Use LoadImage() to get the image loaded into a DIBSection
   *phBitmap = (HBITMAP)LoadImage( NULL, szFileName, IMAGE_BITMAP, 0, 0,
               LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE );

   if( *phBitmap == NULL )
     return FALSE;

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
   return TRUE;

}







