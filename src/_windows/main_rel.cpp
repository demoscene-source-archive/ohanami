
#pragma warning( disable : 4730 )
#pragma warning( disable : 4799 )

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include "../config.h"
#include <GL/gl.h>
#include "../glext.h"
#include "../shader_code.h"
#include "../4klang.h"

#define WRITEBITMAPS 0
#define WRITEWAV 0

#include "../writebitmaps.h"


// MAX_SAMPLES gives you the number of samples for the whole song. we always produce stereo samples, so times 2 for the buffer
SAMPLE_TYPE	lpSoundBuffer[MAX_SAMPLES*2];  
HWAVEOUT	hWaveOut;

#pragma data_seg(".wavefmt")
WAVEFORMATEX WaveFMT =
{
#ifdef FLOAT_32BIT	
	WAVE_FORMAT_IEEE_FLOAT,
#else
	WAVE_FORMAT_PCM,
#endif		
    2, // channels
    SAMPLE_RATE, // samples per sec
    SAMPLE_RATE*sizeof(SAMPLE_TYPE)*2, // bytes per sec
    sizeof(SAMPLE_TYPE)*2, // block alignment;
    sizeof(SAMPLE_TYPE)*8, // bits per sample
    0 // extension not needed
};

#pragma data_seg(".wavehdr")
WAVEHDR WaveHDR = 
{
	(LPSTR)lpSoundBuffer, 
	MAX_SAMPLES*sizeof(SAMPLE_TYPE)*2,			// MAX_SAMPLES*sizeof(float)*2(stereo)
	0, 
	0, 
	0, 
	0, 
	0, 
	0
};

MMTIME MMTime = 
{ 
	TIME_SAMPLES,
	0
};



static const PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
    32, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 32, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };

static DEVMODE screenSettings = { {0},
    #if _MSC_VER < 1400
    0,0,148,0,0x001c0000,{0},0,0,0,0,0,0,0,0,0,{0},0,32,XRES,YRES,0,0,      // Visual C++ 6.0
    #else
    0,0,156,0,0x001c0000,{0},0,0,0,0,0,{0},0,32,XRES,YRES,{0}, 0,           // Visuatl Studio 2005
    #endif
    #if(WINVER >= 0x0400)
    0,0,0,0,0,0,
    #if (WINVER >= 0x0500) || (_WIN32_WINNT >= 0x0400)
    0,0
    #endif
    #endif
    };

#ifdef __cplusplus
extern "C" 
{
#endif
int  _fltused = 1;
#ifdef __cplusplus
}
#endif

//----------------------------------------------------------------------------

static const char *strs[] = {
	"glCreateShaderProgramv",
	"glUseProgram",
	"glGetUniformLocation",
	"glUniform1i",
    };

#define NUMFUNCIONES (sizeof(strs)/sizeof(strs[0]))

#define oglCreateShaderProgramv	      ((PFNGLCREATESHADERPROGRAMVPROC)myglfunc[0])
#define oglUseProgram									((PFNGLUSEPROGRAMPROC)myglfunc[1])
#define oglGetUniformLocation					((PFNGLGETUNIFORMLOCATIONPROC)myglfunc[2])
#define oglUniform1i									((PFNGLUNIFORM1IPROC)myglfunc[3])

#define USE_SOUND_THREAD

//#pragma code_seg(".initsnd")
void  InitSound()
{
#ifdef USE_SOUND_THREAD
	// thx to xTr1m/blu-flame for providing a smarter and smaller way to create the thread :)
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_4klang_render, lpSoundBuffer, 0, 0);
#else
//	_4klang_render(lpSoundBuffer);
#endif


	// SILENCE
#if 0
	for(long i=0;i<MAX_SAMPLES;++i)
	{
		lpSoundBuffer[i*2+0]=lpSoundBuffer[i*2+1]=0;
	}
#endif


	for(long i=0;i<MAX_SAMPLES;++i)
	{
		lpSoundBuffer[i*2+0]=lpSoundBuffer[i*2+1]=0;//(float)buf4;
	}

	waveOutOpen			( &hWaveOut, WAVE_MAPPER, &WaveFMT, NULL, 0, CALLBACK_NULL );
	waveOutPrepareHeader( hWaveOut, &WaveHDR, sizeof(WaveHDR) );
	waveOutWrite		( hWaveOut, &WaveHDR, sizeof(WaveHDR) );

}

void entrypoint( void )
{
    // full screen
    //if( ChangeDisplaySettings(&screenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL) return;
    ChangeDisplaySettings(&screenSettings,CDS_FULLSCREEN);
    ShowCursor( 0 );
    // create window
    HWND hWnd = CreateWindow( "edit",0,WS_POPUP|WS_VISIBLE,0,0,XRES,YRES,0,0,0,0);
//    if( !hWnd ) return;
    HDC hDC = GetDC(hWnd);
    // initalize opengl
    SetPixelFormat(hDC,ChoosePixelFormat(hDC,&pfd),&pfd);
    //if( !SetPixelFormat(hDC,ChoosePixelFormat(hDC,&pfd),&pfd) ) return;
    wglMakeCurrent(hDC,wglCreateContext(hDC));

	void *myglfunc[NUMFUNCIONES];

    for( int i=0; i<NUMFUNCIONES; i++ )
    {
        #ifdef WIN32
        myglfunc[i] = wglGetProcAddress( strs[i] );
        #endif
        #ifdef LINUX
        myglfunc[i] = glXGetProcAddress( (const unsigned char *)strs[i] );
        #endif
    }

		static const char* shp = fragment_glsl;

auto prog = oglCreateShaderProgramv(GL_FRAGMENT_SHADER, 1, &shp);
	oglUseProgram(prog);


oglUniform1i(oglGetUniformLocation(prog, U_TEX0), 0);

#if WRITEWAV
{
		_4klang_render(lpSoundBuffer);

    HANDLE hf = CreateFile("waveout.raw", 
                   GENERIC_READ | GENERIC_WRITE, 
                   (DWORD) 0, 
                    NULL, 
                   CREATE_ALWAYS, 
                   FILE_ATTRIBUTE_NORMAL, 
                   (HANDLE) NULL); 
    DWORD dwTmp; 

    // Copy the BITMAPFILEHEADER into the .BMP file.  
    if (!WriteFile(hf, lpSoundBuffer, sizeof(lpSoundBuffer), 
        (LPDWORD) &dwTmp,  NULL)) 
    {
    }

		ExitProcess(0);
}
#else

#if !WRITEBITMAPS
	InitSound();
#endif

#endif

    do 
    {
//static int frame=0;
//const float time=Clinkster_GetPosition() / Clinkster_TicksPerSecond;
//const float time=float(frame++)/8.f;

#if WRITEBITMAPS
static long int counter=0;
		const long int t = (counter * SAMPLE_RATE) / 60; // 60hz
//		const long int t = (counter * SAMPLE_RATE); // 1hz
MMTime.u.sample=t;
++counter;
#else
		waveOutGetPosition(hWaveOut, &MMTime, sizeof(MMTIME));
		long int t = MMTime.u.sample;
		//long int t = (long int)(((MMTime.u.sample) * 1000) / SAMPLE_RATE);
#endif

		const float time = float(MMTime.u.sample) / SAMPLE_RATE;

		glTexCoord4f(time,0,0,1);
//		glTexCoord1f(time);
		glRects(-1,-1,+1,+1);

		glBindTexture(GL_TEXTURE_2D, 1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);		
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, XRES, YRES, 0);

//		glViewport(0,0,XRES,YRES);
		glTexCoord4f(time,0,0,0);
//		glTexCoord1f(time);
		glRects(-1,-1,+1,+1);
		glFinish();

#if WRITEBITMAPS
static unsigned char framepixels[XRES*YRES*4];
glReadBuffer(GL_BACK);
glPixelStorei(GL_PACK_ALIGNMENT, 1);
glReadPixels(0,0,XRES,YRES,GL_BGRA,GL_UNSIGNED_BYTE,framepixels);
//for(int i=0;i<sizeof(framepixels);++i)framepixels[i]=255+i;
for(int y=0;y<(YRES+1)/2;++y)for(int x=0;x<XRES;++x)for(int c=0;c<4;++c){auto b=framepixels[(x+y*XRES)*4+c];framepixels[(x+y*XRES)*4+c]=framepixels[(x+(YRES-1-y)*XRES)*4+c];framepixels[(x+(YRES-1-y)*XRES)*4+c]=b;}
HBITMAP bitmap=CreateBitmap(XRES,YRES,1,32,framepixels);
PBITMAPINFO bitmapinfo=CreateBitmapInfoStruct(hWnd, bitmap);
char filename[1024];
wsprintf(filename, "frames\\frame%06d.bmp", counter);
CreateBMPFile(hWnd, filename, bitmapinfo, bitmap, hDC);
DeleteObject(bitmap);
#endif

//        SwapBuffers( hDC );
        wglSwapLayerBuffers( hDC, WGL_SWAP_MAIN_PLANE ); //SwapBuffers( hDC );

		PeekMessageA(0, 0, 0, 0, PM_REMOVE); // <-- "fake" message handling.

 //   }while ( !GetAsyncKeyState(VK_ESCAPE) );
    //}while ( !GetAsyncKeyState(VK_ESCAPE) && t<(MZK_DURATION*1000) );
	} while (MMTime.u.sample < MAX_SAMPLES && !GetAsyncKeyState(VK_ESCAPE));

    #ifdef CLEANDESTROY
    sndPlaySound(0,0);
    ChangeDisplaySettings( 0, 0 );
    ShowCursor(1);
    #endif

    ExitProcess(0);
	}
