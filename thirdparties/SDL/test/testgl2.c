/*
  Copyright (C) 1997-2011 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ode/ode.h>
#include <assert.h>
#include "common.h"
#ifdef __MACOS__
#define HAVE_OPENGL
#endif

#ifdef HAVE_OPENGL

#include "SDL_opengl.h"
#include "Vertex.h"
#include "objloader.h"
#include "dds.h"
#include "matrix.h"
#include "font.h"
#include "texture.h"
#include "pushbutton.h"
#include "../../mmo4/clientlogic/clientlogic.h"
#define VELOCITY (10.0f)
#define JUMPVELOCITY (20.0f)

PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = NULL;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = NULL;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = NULL;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLGENBUFFERSPROC glGenBuffers = NULL;
PFNGLBUFFERDATAPROC glBufferData = NULL;
PFNGLBINDBUFFERPROC glBindBuffer = NULL;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = NULL;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = NULL;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = NULL;
PFNGLACTIVETEXTUREPROC glActiveTexture = NULL;
PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D = NULL;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap = NULL;

unsigned int nlpo2(unsigned int x)
{
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x+1;
}

float clampf(float v, float lo, float hi)
{
	return v < lo ? lo : (v > hi ? hi : v);
}

void GL_Load_Extensions(void)
{
	glCreateShader = (PFNGLCREATESHADERPROC)SDL_GL_GetProcAddress("glCreateShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)SDL_GL_GetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)SDL_GL_GetProcAddress("glCompileShader");
	glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteBuffers");
	glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)SDL_GL_GetProcAddress("glDeleteVertexArrays");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)SDL_GL_GetProcAddress("glUniformMatrix4fv");
	glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)SDL_GL_GetProcAddress("glBindVertexArray");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)SDL_GL_GetProcAddress("glCreateProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)SDL_GL_GetProcAddress("glAttachShader");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)SDL_GL_GetProcAddress("glBindAttribLocation");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)SDL_GL_GetProcAddress("glLinkProgram");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)SDL_GL_GetProcAddress("glGetUniformLocation");
	glUseProgram = (PFNGLUSEPROGRAMPROC)SDL_GL_GetProcAddress("glUseProgram");
	glDeleteShader = (PFNGLDELETESHADERPROC)SDL_GL_GetProcAddress("glDeleteShader");
	glGenBuffers = (PFNGLGENBUFFERSPROC)SDL_GL_GetProcAddress("glGenBuffers");
	glBufferData = (PFNGLBUFFERDATAPROC)SDL_GL_GetProcAddress("glBufferData");
	glBindBuffer = (PFNGLBINDBUFFERPROC)SDL_GL_GetProcAddress("glBindBuffer");
	glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)SDL_GL_GetProcAddress("glGenVertexArrays");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)SDL_GL_GetProcAddress("glVertexAttribPointer");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)SDL_GL_GetProcAddress("glEnableVertexAttribArray");
	glActiveTexture = (PFNGLACTIVETEXTUREPROC)SDL_GL_GetProcAddress("glActiveTexture");
	glCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2DPROC)SDL_GL_GetProcAddress("glCompressedTexImage2D");
	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)SDL_GL_GetProcAddress("glGenerateMipmap");
}

/* Undefine this if you want a flat cube instead of a rainbow cube */
//#define SHADED_CUBE

static CommonState *state;
static SDL_GLContext context;

typedef struct KeyControlVector
{
	unsigned char left:1;
	unsigned char right:1;
	unsigned char up:1;
	unsigned char down:1;
	unsigned char jump:1;
} KeyControlVector;

typedef __declspec(align(32)) struct Object
{
	float x, y, z, w;
} Object;

static VertexBuffer vb, vblightmap, vbui;
GLuint tex, texlightmap, texui, dyntexttexui, dyntexttexui2;
static KeyControlVector kcv;
static GLint mvp;
static Object __declspec(align(32)) player;
static float camdist = 30.0f;
static float campitch = (float)(M_PI / 4.0f);
static float camyaw = 0;
static dSpaceID space;
static dWorldID world;
static dGeomID playergeom;
static dBodyID playerbody;
static dJointGroupID contactgroup;
static int playeronground;
const static float MAXSTEP = 1.0f/60;
static float dtvar; // dtvar는 dt 값부터 시작해서 0으로 줄어드는 변수
static BOOL uimode = FALSE;
static float __declspec(align(32)) uipos[4];
static float __declspec(align(32)) uiscale[4] = { 200.0, 200.0, 1, 1 };
static int fsaa = 0;
static int accel = -1;
static int cursor;
static int selection_len;

static wchar_t gUserNameText[1024] = {0};
static wchar_t gCompositingUserNameText[32] = {0};
static unsigned long __declspec(align(4)) gUserNameTextDirtyCount;

static wchar_t gWorldNameText[1024] = {0};
static wchar_t gCompositingWorldNameText[32] = {0};
static unsigned long __declspec(align(4)) gWorldNameTextDirtyCount;

static wchar_t* gImeText = gUserNameText;
static wchar_t* gCompositingText = gCompositingUserNameText;
static unsigned long* gTextDirtyCount = &gUserNameTextDirtyCount;

static const char* string_from_text_file(const char* filename)
{
	HANDLE handle;
	LARGE_INTEGER s;
	char* text;
	DWORD read;

	handle = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);

	GetFileSizeEx(handle, &s);

	if (s.HighPart != 0)
		return NULL;
    
	text = (char*)malloc(s.LowPart + 1);
	
	ReadFile(handle, text, s.LowPart, &read, NULL);

	CloseHandle(handle);

	text[s.LowPart] = '\0';
     
    return text;
}

static GLuint create_shader_from_file(GLenum type, const char* filename)
{
    const char* shader_text_ptr = string_from_text_file(filename);
	GLuint name;
	
	if (shader_text_ptr == NULL)
		return 0;

	puts(shader_text_ptr);

    name = glCreateShader(type);

    glShaderSource(name, 1, &shader_text_ptr, 0);
    glCompileShader(name);

	free((void*)shader_text_ptr);

    return name;
}

/* Call this instead of exit(), so we can clean up SDL: atexit() is evil. */
static void
quit(int rc)
{
	// 이 미친 NVIDIA OpenGL 드라이버가 메모리 해제를 안해주고 프로그램 끝내면 크래시를 내네? ㅋㅋ
	glDeleteBuffers(1, &vb.vbuffer);
	glDeleteVertexArrays(1, &vb.varray);

	glDeleteBuffers(1, &vblightmap.vbuffer);
	glDeleteVertexArrays(1, &vblightmap.varray);

	glDeleteBuffers(1, &vbui.vbuffer);
	glDeleteVertexArrays(1, &vbui.varray);

    if (context) {
        /* SDL_GL_MakeCurrent(0, NULL); *//* doesn't do anything */
        SDL_GL_DeleteContext(context);
    }

    CommonQuit(state);
    exit(rc);
}

static void nearCallback (void *data, dGeomID o1, dGeomID o2)
{
	int i, n;

	const char* o1name;
	const char* o2name;
	int o1c, o2c;
	dContact contact[100];

	dBodyID b1 = dGeomGetBody(o1);
	dBodyID b2 = dGeomGetBody(o2);
	if (b1 && b2 && dAreConnected(b1, b2))
		return;

	o1name = (const char*) dGeomGetData (o1);
	o2name = (const char*) dGeomGetData (o2);

	// 벽끼리 충돌은 배제
	if (dGeomGetClass(o1) == dPlaneClass && dGeomGetClass(o2) == dPlaneClass)
		return;

	o1c = dGeomGetClass(o1);
	o2c = dGeomGetClass(o2);

	//printf ("collision (%s, %s)\n", o1name, o2name);

	n = dCollide (o1,o2,_countof(contact),&contact[0].geom,sizeof(dContact));

	if ((strcmp(o1name, "Player") == 0 && strcmp(o2name, "Ground") == 0)
		|| strcmp(o2name, "Player") == 0 && strcmp(o1name, "Ground") == 0)
	{
		if (n > 0)
			playeronground = 1;
	}

	if (n > 0) {
		for (i=0; i<n; i++) {
			dJointID c;

			contact[i].surface.mode = 0;//dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;
			//if (dGeomGetClass(o1) == dSphereClass || dGeomGetClass(o2) == dSphereClass)
			//	contact[i].surface.mu = 20;
			//else
			//	contact[i].surface.mu = 0.5;
			//contact[i].surface.mu = dInfinity;
			contact[i].surface.mu = 0;
			contact[i].surface.slip1 = 0.0;
			contact[i].surface.slip2 = 0.0;
			contact[i].surface.soft_erp = 0.0;
			contact[i].surface.soft_cfm = 0.01f;
			
			c = dJointCreateContact (world,contactgroup,contact+i);
			dJointAttach (c,dGeomGetBody(o1),dGeomGetBody(o2));
		}
	}
}

static void
	UpdateDynamicText(
	_In_ _Notnull_ unsigned long* dirtyCount,
	_In_ _Notnull_ wchar_t* imeText,
	_In_ _Notnull_ wchar_t* compositingText,
	GLuint texId,
	int x,
	int iy
	)
{
	if (*dirtyCount > 0UL)
	{
		int i;
		const int dynfontsize = 64;
		const int dynfonttexsize = 512;
		char* textrenderdata = (char*)calloc(4, dynfonttexsize*dynfonttexsize);
		wchar_t renderText[4096] = {0};

		//memset(textrenderdata, 0xff, 4*dynfonttexsize*dynfonttexsize);
		for (i = 0; i < dynfonttexsize*dynfonttexsize; ++i)
		{
			//textrenderdata[4*i + 3] = 0xff;
		}

		wcscat(renderText, imeText);
		wcscat(renderText, compositingText);
			
		DrawTextToMemory(textrenderdata, dynfonttexsize, dynfonttexsize, renderText, x, dynfontsize-1 + iy, dynfontsize);
	
		glBindTexture(GL_TEXTURE_2D, texId);
		glActiveTexture(GL_TEXTURE0);
		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			0,
			0,
			dynfonttexsize,
			dynfonttexsize,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			textrenderdata
			);
		glGenerateMipmap(GL_TEXTURE_2D);

		free(textrenderdata);

		InterlockedDecrement(dirtyCount);
	}
}

static void
MyRender(float dt, int w, int h)
{
	static float __declspec(align(32)) m[16], v[16], p[16], vscale[16], mvpdata[16];
	float __declspec(align(32)) camtrans[] = {0,0,-camdist,0};
	float campitchaxis[] = {1.0f,0,0,0};
	float camyawaxis[] = {0.0f,0.0f,1.0f,0};
	float playerscale[4] = { 1, 1, 1, 1 };

	glhLoadIdentityf2(v);
	//glhLookAtf2(v, eye, at, up);
	glhTranslatef2_SSE_Aligned(v, camtrans);
	glhRotatef2_SSE_Aligned(v, -campitch, campitchaxis);
	glhRotatef2_SSE_Aligned(v, -camyaw, camyawaxis);
	
	
	
	
	glhLoadIdentityf2(p);
	glhPerspectivef2(p, 45, (float)w/h, 0.1f, 1000.0f);
	
	
    glClearColor(0.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Level //////////////////////////////////////////////////////
	
	glhLoadIdentityf2(m);
	//glhTranslatef2_SSE_Aligned(m, &player.x);
		
	//glhMultMatrixSwappedf2_SSE_Aligned
	glhLoadIdentityf2(mvpdata);
	glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, m);
	glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, v);
	glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, p);

	glUniformMatrix4fv(mvp, 1, GL_FALSE, (GLfloat*)mvpdata);
	
	glBindTexture(GL_TEXTURE_2D, texlightmap);

	glBindVertexArray(vblightmap.varray);
	glDrawArrays(GL_TRIANGLES, 0, vblightmap.len);


	// Plane //////////////////////////////////////////////////////
	
	glhLoadIdentityf2(m);
	glhTranslatef2_SSE_Aligned(m, &player.x);
	glhScalef2_SSE_Aligned(m, playerscale);
	
	//glhMultMatrixSwappedf2_SSE_Aligned
	glhLoadIdentityf2(mvpdata);
	glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, m);
	glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, v);
	glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, p);

	glUniformMatrix4fv(mvp, 1, GL_FALSE, (GLfloat*)mvpdata);

	glBindTexture(GL_TEXTURE_2D, tex);
	
	glBindVertexArray(vb.varray);
	glDrawArrays(GL_TRIANGLES, 0, vb.len);

	// UI Layer ///////////////////////////////
	
	if (uimode)
	{
		float eye[4] = { 0, 0, 0.5, 0 };
		float at[4] = {0, 0, 0, 0};
		float up[4] = {0, 1, 0, 0};
		float uiviewscale[4] = { 2.0f / w, 2.0f / h, 1.0f, 1.0f };
		float uiviewscaleinv[4] = { w / 2.0f, h / 2.0f, 1.0f, 1.0f };
		float uiviewtrans[4] = { -w/2.0f, -h/2.0f, 0, 0 };
		/*int x, y;
		
		SDL_GetMouseState(&x, &y);
		uipos[0] = (float)(x-128);
		uipos[1] = (float)(h-y-128);*/

		
		glhLoadIdentityf2(m);
		glhTranslatef2_SSE_Aligned(m, uipos);
		glhScalef2_SSE_Aligned(m, uiscale);
		
		//glhLoadIdentityf2(vscale);
		//glhScalef2_SSE_Aligned(vscale, uiviewscale);

		glhLoadIdentityf2(v);
		//glhScalef2_SSE_Aligned(v, uiviewscaleinv);
		
		glhScalef2_SSE_Aligned(v, uiviewscale);
		glhTranslatef2_SSE_Aligned(v, uiviewtrans);
		


		glhLoadIdentityf2(p);
		//glhOrthof2(p, 0, 0, (float)w, (float)h, 0, 1.0f);

		glhLoadIdentityf2(mvpdata);
		glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, m);
		glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, v);
		glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, p);

		// modify texture according to user text input

		UpdateDynamicText(&gUserNameTextDirtyCount, gUserNameText, gCompositingUserNameText, dyntexttexui, 0, 300);
		UpdateDynamicText(&gWorldNameTextDirtyCount, gWorldNameText, gCompositingWorldNameText, dyntexttexui2, 0, 100);



		glDisable(GL_DEPTH_TEST);

		glUniformMatrix4fv(mvp, 1, GL_FALSE, (GLfloat*)mvpdata);
		glBindTexture(GL_TEXTURE_2D, texui);
		glBindVertexArray(vbui.varray);
		glDrawArrays(GL_TRIANGLES, 0, vbui.len);

		glBindTexture(GL_TEXTURE_2D, dyntexttexui);
		//glBindVertexArray(vbui.varray);
		glDrawArrays(GL_TRIANGLES, 0, vbui.len);

		glBindTexture(GL_TEXTURE_2D, dyntexttexui2);
		//glBindVertexArray(vbui.varray);
		glDrawArrays(GL_TRIANGLES, 0, vbui.len);

		glEnable(GL_DEPTH_TEST);
	}

}

static int on_spawn_pkt(int id, double x, double y)
{
	return 0;
}

DWORD WINAPI CallMeThreadProc(_In_  LPVOID lpParameter)
{
	return 1234;
}

int SDLCALL RenderThread(void *data)
{
	SDL_DisplayMode mode;
    int status;
	GLuint vs, fs, program;
	VertexData3D vd3d[] = {
		{0-0.5f,0-0.5f,0, 0,0, 0,0,1},
		{1-0.5f,0-0.5f,0, 1,0, 0,0,1},
		{0-0.5f,1-0.5f,0, 0,1, 0,0,1},
		{1-0.5f,0-0.5f,0, 1,0, 0,0,1},
		{1-0.5f,1-0.5f,0, 1,1, 0,0,1},
		{0-0.5f,1-0.5f,0, 0,1, 0,0,1},
	};
    int value;

    /* Create OpenGL context */
    context = SDL_GL_CreateContext(state->windows[0]);
    if (!context) {
        fprintf(stderr, "SDL_GL_CreateContext(): %s\n", SDL_GetError());
        quit(2);
    }

    if (state->render_flags & SDL_RENDERER_PRESENTVSYNC) {
        /* try late-swap-tearing first. If not supported, try normal vsync. */
        if (SDL_GL_SetSwapInterval(-1) == -1) {
            SDL_GL_SetSwapInterval(1);
        }
    } else {
        SDL_GL_SetSwapInterval(0);  /* disable vsync. */
    }
	
    SDL_GetCurrentDisplayMode(0, &mode);
    printf("Screen BPP    : %d\n", SDL_BITSPERPIXEL(mode.format));
    printf("Swap Interval : %d\n", SDL_GL_GetSwapInterval());
    printf("\n");
    printf("Vendor        : %s\n", glGetString(GL_VENDOR));
    printf("Renderer      : %s\n", glGetString(GL_RENDERER));
    printf("Version       : %s\n", glGetString(GL_VERSION));
    printf("\n");

    status = SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &value);
    if (!status) {
        printf("SDL_GL_RED_SIZE: requested %d, got %d\n", 5, value);
    } else {
        printf("Failed to get SDL_GL_RED_SIZE: %s\n", SDL_GetError());
    }
    status = SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &value);
    if (!status) {
        printf("SDL_GL_GREEN_SIZE: requested %d, got %d\n", 5, value);
    } else {
        printf("Failed to get SDL_GL_GREEN_SIZE: %s\n", SDL_GetError());
    }
    status = SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &value);
    if (!status) {
        printf("SDL_GL_BLUE_SIZE: requested %d, got %d\n", 5, value);
    } else {
        printf("Failed to get SDL_GL_BLUE_SIZE: %s\n", SDL_GetError());
    }
    status = SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value);
    if (!status) {
        printf("SDL_GL_DEPTH_SIZE: requested %d, got %d\n", 16, value);
    } else {
        printf("Failed to get SDL_GL_DEPTH_SIZE: %s\n", SDL_GetError());
    }
    if (fsaa) {
        status = SDL_GL_GetAttribute(SDL_GL_MULTISAMPLEBUFFERS, &value);
        if (!status) {
            printf("SDL_GL_MULTISAMPLEBUFFERS: requested 1, got %d\n", value);
        } else {
            printf("Failed to get SDL_GL_MULTISAMPLEBUFFERS: %s\n",
                   SDL_GetError());
        }
        status = SDL_GL_GetAttribute(SDL_GL_MULTISAMPLESAMPLES, &value);
        if (!status) {
            printf("SDL_GL_MULTISAMPLESAMPLES: requested %d, got %d\n", fsaa,
                   value);
        } else {
            printf("Failed to get SDL_GL_MULTISAMPLESAMPLES: %s\n",
                   SDL_GetError());
        }
    }
    if (accel >= 0) {
        status = SDL_GL_GetAttribute(SDL_GL_ACCELERATED_VISUAL, &value);
        if (!status) {
            printf("SDL_GL_ACCELERATED_VISUAL: requested %d, got %d\n", accel,
                   value);
        } else {
            printf("Failed to get SDL_GL_ACCELERATED_VISUAL: %s\n",
                   SDL_GetError());
        }
    }

    /* Set rendering settings */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2.0, 2.0, -2.0, 2.0, -20.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
	//glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);



	GL_Load_Extensions();

	vs = create_shader_from_file(GL_VERTEX_SHADER, "default.vert");
	fs = create_shader_from_file(GL_FRAGMENT_SHADER, "default.frag");

	program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);


	glBindAttribLocation(program, POSITION, "Position");
    glBindAttribLocation(program, TEXCOORD, "Texcoord");
	glBindAttribLocation(program, NORMAL, "Normal");
    
	glLinkProgram(program);

	mvp		 = glGetUniformLocation(program, "MVP");
	
	
	load_obj(&vb, "capsule.obj");
	
	glUseProgram(program);
	//glLoadIdentity();
	//glFrustum(-30,30,-30,30,0.1,100);

	tex = load_dds("capsule.dds");
	texlightmap = load_dds("lightmap.dds");

	load_obj(&vblightmap, "lightmap.obj");


	vbui.data = vd3d;
	vbui.len = _countof(vd3d);
	create_vertex_buffer_from_array_3d(&vbui);

	// FreeType 이용해서 UI 창 만들어보자
	{
		int i, j;
		int fontsize = 32;
		char* texdata;
		int texsize = 512;
		const wchar_t* wintitle = L"시작하기";
		int mainborderthickness = texsize / 100;
		int subborderthickness = mainborderthickness / 2;
		int titlemargin = mainborderthickness;

		// Plate color: 0xff333333
		
		//texsize = nlpo2(pitch) < nlpo2(rows) ? nlpo2(rows) : nlpo2(pitch);
		//texsize = 256; // 1024x1024 판떼기

		texdata = (char*)calloc(4, texsize * texsize);

		dyntexttexui = CreateTextureFromMemory(texsize, texsize, texdata);
		dyntexttexui2 = CreateTextureFromMemory(texsize, texsize, texdata);

		//memset(texdata, 0x7f, 4 * texsize * texsize);
		
		uiscale[0] = 256;
		uiscale[1] = 256;

		{
			int w, h;
			SDL_GetWindowSize(state->windows[0], &w, &h);
			uipos[0] = w/2.0f;
			uipos[1] = h/2.0f;
		}

		for (i = 0; i < texsize; ++i)
		{
			for (j = 0; j < texsize; ++j)
			{
				char c[4];

				if (j < mainborderthickness)
				{
					c[0] = c[1] = c[2] = 0x27; c[3] = 0xff;
				}
				else if (j > texsize - mainborderthickness)
				{
					c[0] = c[1] = c[2] = 0x27; c[3] = 0xff;
				}
				else
				{

					if (i > texsize - mainborderthickness)
					{
						c[0] = c[1] = c[2] = 0x27; c[3] = 0xff;
					}
					else if (i > texsize - mainborderthickness - mainborderthickness)
					{
						c[0] = c[1] = c[2] = 0x58; c[3] = 0xff;
					}
					else if (i > texsize - mainborderthickness - mainborderthickness - (fontsize + titlemargin*2))
					{	
						c[0] = c[1] = c[2] = 0x33; c[3] = 0xff;
					}
					else if (i > texsize - mainborderthickness - mainborderthickness - (fontsize + titlemargin*2) - subborderthickness)
					{
						c[0] = c[1] = c[2] = 0x27; c[3] = 0xff;
					}
					else if (i > texsize - mainborderthickness - mainborderthickness - (fontsize + titlemargin*2) - subborderthickness - subborderthickness)
					{
						c[0] = c[1] = c[2] = 0x58; c[3] = 0xff;
					}
					else if (i > mainborderthickness + mainborderthickness)
					{	
						c[0] = c[1] = c[2] = 0x24; c[3] = 0xff;
					}
					else if (i > mainborderthickness)
					{
						c[0] = c[1] = c[2] = 0x27; c[3] = 0xff;
					}
					else
					{
						c[0] = c[1] = c[2] = 0x58; c[3] = 0xff;
					}
				}

				texdata[4*(texsize * i + j) + 0] = c[0];
				texdata[4*(texsize * i + j) + 1] = c[1];
				texdata[4*(texsize * i + j) + 2] = c[2];
				texdata[4*(texsize * i + j) + 3] = c[3];
			}
		}

		/*for (i = 0; i < texsize; ++i)
		{
			for (j = texsize-20; j < texsize; ++j)
			{
				texdata[4*(texsize * i + j) + 2] = 255;
				texdata[4*(texsize * i + j) + 3] = 255;
			}
		}

		for (i = 0; i < 20; ++i)
		{
			for (j = 0; j < texsize; ++j)
			{
				texdata[4*(texsize * i + j) + 2] = 255;
				texdata[4*(texsize * i + j) + 3] = 255;
			}
		}*/

		DrawTextToMemory(
			texdata,
			texsize,
			texsize,
			wintitle,
			titlemargin*2,
			texsize - mainborderthickness - titlemargin*2,
			fontsize
			);

		DrawTextToMemory(
			texdata,
			texsize,
			texsize,
			L"이름",
			titlemargin*2 + titlemargin*2,
			texsize - mainborderthickness - titlemargin*2 - fontsize*2,
			fontsize
			);

		DrawTextToMemory(
			texdata,
			texsize,
			texsize,
			L"세계",
			titlemargin*2 + titlemargin*2,
			texsize - mainborderthickness - titlemargin*2 - fontsize*8,
			fontsize
			);

		DrawPushButtonToMemory(
			texdata,
			texsize,
			texsize,
			300,
			20,
			150,
			50,
			2,
			L"시작",
			30);

		texui = CreateTextureFromMemory(texsize, texsize, texdata);
		
		free(texdata);
	}

	while (1)
	{
        int w, h;
        //SDL_GL_MakeCurrent(state->windows[i], context);
        SDL_GetWindowSize(state->windows[0], &w, &h);
        //glViewport(0, 0, w, h);
            
		MyRender(0.016f, w, h);

		glDrawBuffer(GL_FRONT);
		SDL_Delay(1);
		glFinish();
	
		//SDL_GL_SwapWindow(state->windows[0]);
    }

	return 0;
}

int SDLCALL PhysicsThread(void *data)
{
	float dt = 0.16;

	dInitODE();
	{
		dMass m;
		dVector3 center = {0,0,0,0};
		dVector3 extents = {30,30,30,0};
		//space = dQuadTreeSpaceCreate(0, center, extents, 7);
		//space = dHashSpaceCreate(0);
		space = dSimpleSpaceCreate(0);
		//space = dSweepAndPruneSpaceCreate( 0, dSAP_AXES_XYZ );
		world = dWorldCreate();
		dWorldSetGravity(world, 0, 0, -50);
		//dWorldSetCFM (world, 1e-5);
		//dWorldSetERP (world, 0.8);
		
		contactgroup = dJointGroupCreate (0);

		
		// Player 설정
		playerbody = dBodyCreate(world);
		dMassSetSphere(&m, 10, 1);
		dMassAdjust(&m, 1);
		dBodySetMass(playerbody, &m);
		//dBodySetAngularDamping(playerbody, 0);
		dBodySetMaxAngularSpeed(playerbody, 0);

		//playergeom = dCreateSphere(space, 1);
		playergeom = dCreateCapsule(space, 1, 6);
		dGeomSetData(playergeom, "Player");
		dGeomSetBody(playergeom, playerbody);
		
		dBodySetPosition(playerbody, 0, 0, 5);
		
		// 사방 네 개의 벽
		dGeomSetData(dCreatePlane(space, -1, 0, 0, -10), "EastWall");
		dGeomSetData(dCreatePlane(space, 1, 0, 0, -10), "WestWall");
		dGeomSetData(dCreatePlane(space, 0, 1, 0, -10), "SouthWall");
		dGeomSetData(dCreatePlane(space, 0, -1, 0, -10), "NorthWall");

		dGeomSetData(dCreatePlane(space, 0, 0, 1, -1), "Ground");
	}

	while (1)
	{
		dtvar += dt;
	
		do
		{
			if (dtvar > MAXSTEP)
			{
				dtvar -= MAXSTEP;

				playeronground = 0;

				dSpaceCollide(space, NULL, &nearCallback);
				
				dWorldStep(world, MAXSTEP);

				dJointGroupEmpty(contactgroup);
			}

		} while (dtvar > MAXSTEP);

		
		{
			const dReal* p = dBodyGetPosition(playerbody);
			const dReal* v = dBodyGetLinearVel(playerbody);

			player.x = p[0];
			player.y = p[1];
			player.z = p[2];
			player.w = 1.0f;

			dBodySetLinearVel(playerbody,
				(kcv.right - kcv.left) * VELOCITY,
				(kcv.up - kcv.down) * VELOCITY,
				((kcv.jump && playeronground) ? JUMPVELOCITY : v[2]));
		}

		/*playeronground = 0;

		dSpaceCollide(space, NULL, &nearCallback);

		dWorldStep(world, 1.0f/60);

		dJointGroupEmpty(contactgroup);*/
	}

	dCloseODE();

	return 0;
}

int
main(int argc, char *argv[])
{
    int i, done;
    SDL_Event event;
    Uint32 then, now, frames;
	int jumpnow = 0;

	Uint64 perf_freq = 0;
	Uint64 perf_count_then = 0, perf_count_prev = 0;
	float dt;

	SDL_Rect input_rect;
	input_rect.x = 10;
	input_rect.y = 20;
	input_rect.w = 100;
	input_rect.h = 200;


	printf("PID: %d   C: 0x%x\n", GetCurrentProcessId(), CallMeThreadProc);

	/* Initialize test framework */
    state = CommonCreateState(argv, SDL_INIT_VIDEO);
    if (!state) {
        return 1;
    }
    for (i = 1; i < argc;) {
        int consumed;

        consumed = CommonArg(state, i);
        if (consumed == 0) {
            if (SDL_strcasecmp(argv[i], "--fsaa") == 0) {
                ++fsaa;
                consumed = 1;
            } else if (SDL_strcasecmp(argv[i], "--accel") == 0 && i+1 < argc) {
                accel = atoi(argv[i+1]);
                consumed = 2;
            } else {
                consumed = -1;
            }
        }
        if (consumed < 0) {
            fprintf(stderr, "Usage: %s %s [--fsaa] [--accel n]\n", argv[0],
                    CommonUsage(state));
            quit(1);
        }
        i += consumed;
    }

    /* Set OpenGL parameters */
    state->window_flags |= SDL_WINDOW_OPENGL;
    state->gl_red_size = 5;
    state->gl_green_size = 5;
    state->gl_blue_size = 5;
    state->gl_depth_size = 16;
    state->gl_double_buffer = 1;
    if (fsaa) {
        state->gl_multisamplebuffers = 1;
        state->gl_multisamplesamples = fsaa;
    }
    if (accel >= 0) {
        state->gl_accelerated = accel;
    }

    if (!CommonInit(state)) {
        quit(2);
    }

	
	// Mouse Grab
	//SDL_ShowCursor(SDL_DISABLE);
	//SDL_SetWindowGrab(state->windows[0], SDL_TRUE);
	
	/* Main render loop */
    frames = 0;
    then = SDL_GetTicks();
    done = 0;

	if (uimode)
	{
		SDL_StartTextInput();
		//SDL_SetTextInputRect(&input_rect);
	}
	else
	{
		SDL_StopTextInput();
	}
	SDL_SetRelativeMouseMode(uimode ? SDL_FALSE : SDL_TRUE);

	//////////////

	glhInit();

	//cl_py_initialize("124.5.229.245", "blen");
	
	InitializeFontSystem();
	

	
	SDL_CreateThread(RenderThread, "RenderThread", NULL);
	SDL_CreateThread(PhysicsThread, "PhysicsThread", NULL);

    while (!done) {
        /* Check for events */
        ++frames;

		jumpnow = 0;

		perf_count_then = SDL_GetPerformanceCounter();

        while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_CLOSE:
					{
						quit(0);
					}
					break; 
				}
				break;
			}

            CommonEvent(state, &event, &done);


			switch (event.type)
            {
                case SDL_TEXTINPUT:
                    /* Add new text onto the end of our text */
					{
						wchar_t textw[4096];

						MultiByteToWideChar(CP_UTF8, 0, event.text.text, strlen(event.text.text) + 1, textw, _countof(textw));

						wcscat(gImeText, textw);

						InterlockedIncrement(gTextDirtyCount);
					}
                    break;
                case SDL_TEXTEDITING:
					{
						/*
						Update the composition text.
						Update the cursor position.
						Update the selection length (if any).
						*/
						wchar_t textw[4096];

						MultiByteToWideChar(CP_UTF8, 0, event.edit.text, strlen(event.edit.text) + 1, textw, _countof(textw));
						cursor = event.edit.start;
						selection_len = event.edit.length;

						wcscpy(gCompositingText, textw);

						/*if (strlen(event.edit.text) > 0)
						{
							MultiByteToWideChar(CP_UTF8, 0, event.edit.text, strlen(event.edit.text) + 1, gCompositingText, _countof(gCompositingText));
							cursor = event.edit.start;
							selection_len = event.edit.length;
						}
						else
						{
							gCompositingText[0] = L'\0';
						}*/

						InterlockedIncrement(gTextDirtyCount);
					}
                    break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym) {
					case SDLK_LEFT:
						if (event.key.repeat == 0)
							cl_entity_command(CAC_ENTITY_MOVE_LEFT, cl_player_id());
						kcv.left = 1;
						break;
					case SDLK_RIGHT:
						if (event.key.repeat == 0)
							cl_entity_command(CAC_ENTITY_MOVE_RIGHT, cl_player_id());
						kcv.right = 1;
						break;
					case SDLK_UP:
						if (event.key.repeat == 0)
							cl_entity_command(CAC_ENTITY_MOVE_UP, cl_player_id());
						kcv.up = 1;
						break;
					case SDLK_DOWN:
						if (event.key.repeat == 0)
							cl_entity_command(CAC_ENTITY_MOVE_DOWN, cl_player_id());
						kcv.down = 1;
						break;
					case SDLK_SPACE:
						if (event.key.repeat == 0)
							jumpnow = 1;
						kcv.jump = 1;
						break;
					case SDLK_BACKSPACE:
						{
							if (SDL_IsTextInputActive())
							{
								size_t ch = wcslen(gImeText);
								size_t chc = wcslen(gCompositingText);
								if (chc > 0)
								{
									gCompositingText[chc-1] = L'\0';

									InterlockedIncrement(gTextDirtyCount);
								}
								else if (ch > 0)
								{
									gImeText[ch-1] = L'\0';

									InterlockedIncrement(gTextDirtyCount);
								}
							}
						}
						break;
					}
					break;
				case SDL_KEYUP:
					switch (event.key.keysym.sym) {
					case SDLK_LEFT:
						kcv.left = 0;

						if ((kcv.left & kcv.right & kcv.up & kcv.down) == 0)
							cl_entity_command(CAC_ENTITY_MOVE_STOP, cl_player_id());
						break;
					case SDLK_RIGHT:
						kcv.right = 0;

						if ((kcv.left & kcv.right & kcv.up & kcv.down) == 0)
							cl_entity_command(CAC_ENTITY_MOVE_STOP, cl_player_id());
						break;
					case SDLK_UP:
						kcv.up = 0;

						if ((kcv.left & kcv.right & kcv.up & kcv.down) == 0)
							cl_entity_command(CAC_ENTITY_MOVE_STOP, cl_player_id());
						break;
					case SDLK_DOWN:
						kcv.down = 0;

						if ((kcv.left & kcv.right & kcv.up & kcv.down) == 0)
							cl_entity_command(CAC_ENTITY_MOVE_STOP, cl_player_id());
						break;
					case SDLK_SPACE:
						kcv.jump = 0;
						break;

					case SDLK_F2:
						SDL_SetRelativeMouseMode( SDL_GetRelativeMouseMode() ? SDL_FALSE : SDL_TRUE );
						uimode = !uimode;

						if (uimode)
							SDL_StartTextInput();
						else
							SDL_StopTextInput();
						break;
					case SDLK_TAB:
						if (uimode)
						{
							if (gImeText == NULL || gImeText == gWorldNameText)
							{
								SDL_StopTextInput();
								SDL_StartTextInput();

								gImeText = gUserNameText;
								gCompositingText = gCompositingUserNameText;
								gTextDirtyCount = &gUserNameTextDirtyCount;
							}
							else
							{
								SDL_StopTextInput();
								SDL_StartTextInput();

								gImeText = gWorldNameText;
								gCompositingText = gCompositingWorldNameText;
								gTextDirtyCount = &gWorldNameTextDirtyCount;
							}
						}
						break;
					case SDLK_RETURN:
						{
							if (wcslen(gUserNameText) > 0 && wcslen(gWorldNameText) > 0)
							{
								char username[4096], worldname[4096];
								WideCharToMultiByte(CP_UTF8, 0, gUserNameText, -1, username, sizeof(username), NULL, NULL);
								WideCharToMultiByte(CP_UTF8, 0, gWorldNameText, -1, worldname, sizeof(worldname), NULL, NULL);

								cl_py_initialize(worldname, username);
								cl_py_set_spawn_callback(on_spawn_pkt);
							}
						}

						break;

					}
					break;

				case SDL_MOUSEWHEEL:
					if (!uimode)
						camdist -= event.wheel.y / 15.0f;
					break;

				case SDL_MOUSEMOTION:
					if (!uimode)
					{
						campitch = clampf(campitch + event.motion.yrel / 250.0f, 0, M_PI/2);
						camyaw = clampf(camyaw + event.motion.xrel / 250.0f, -M_PI, M_PI);
					}
					break;
			}
        }

		perf_freq = SDL_GetPerformanceFrequency();

		dt = (float)perf_count_prev / perf_freq;


		//dGeomSetPosition(playergeom, player.x, player.y, player.z);

		cl_frame_move(dt);

        for (i = 0; i < state->num_windows; ++i) {
            int w, h;
            //SDL_GL_MakeCurrent(state->windows[i], context);
            SDL_GetWindowSize(state->windows[i], &w, &h);
            //glViewport(0, 0, w, h);
            
			//Render();
			
			// MyRender(dt, w, h);
            
			//glDrawBuffer(GL_FRONT);
			//Sleep(0);
			//glFinish();

			// Sleep(10);
			
			// SDL_GL_SwapWindow(state->windows[i]);
			
        }

		perf_count_prev = SDL_GetPerformanceCounter() - perf_count_then;
	}

    /* Print out some timing information */
    now = SDL_GetTicks();
    if (now > then) {
        printf("%2.2f frames per second\n",
               ((double) frames * 1000) / (now - then));
    }
    quit(0);
    return 0;
}



#else /* HAVE_OPENGL */

int
main(int argc, char *argv[])
{
    printf("No OpenGL support on this system\n");
    return 1;
}

#endif /* HAVE_OPENGL */
