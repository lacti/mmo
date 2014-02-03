#include "stdafx.h"
#include "renderingmodule.h"
#include "../mmobase/matrix.h"
#include "../mmobase/basicmath.h"
#include "../rendering_service/vertex.h"
#include "../rendering_service/dds.h"
#include "../rendering_service/texture.h"
#include "../rendering_service/balloon.h"
#include "../rendering_service/shadowmap.h"
#include "../rendering_service/objloader.h"
#include "../rendering_service/simplestaticobject.h"
#include "../rendering_service/defaultshaderprogram.h"
#include "../rendering_service/rocketshaderprogram.h"
#include "common.h"
#include "object.h"
#include "shader.h"
#include "screentext.h"
#include "winsys.h"
#include "rocket.h"
#include <stdlib.h>
#include <algorithm>
#include <iterator>
#include <random>

extern int fsaa;
extern int accel;
extern Object player;
extern BOOL gUiMode;
extern BOOL gChatMode;
extern unsigned long __declspec(align(4)) gRedrawCurrentIme;
extern wchar_t gUserNameText[1024];
extern wchar_t gCompositingUserNameText[32];
extern wchar_t gWorldNameText[1024];
extern wchar_t gCompositingWorldNameText[32];
extern wchar_t gChatLineText[256];
extern wchar_t gCompositingChatLineText[32];
extern float camdist;
extern float campitch;
extern float camyaw;
extern int gFire;
typedef boost::circular_buffer<std::wstring> ChatLogBuffer;
extern ChatLogBuffer gChatLog;
shadowmap_conf sm_conf;

int gDoRenderingLoop = 1;
VertexBuffer vbui, vbchar, vbpie;
GLint gUniMvp, gUniUvx, gUniAlpha, gUniDirLight, gUniMvpShadowMap, gUniShadowMap;
GLuint gMainProgram;
SimpleStaticObject gRockCylinder;
VertexBuffer* testVbBuf = nullptr;
int testVbBufSize = 0;

#define TERRAIN_SEG_X (4)
#define TERRAIN_SEG_Y (4)

struct TerrainSegment
{
    SDL_Point segIndex;
    VertexBuffer vertbuf;
};

static SDL_GLContext context;
static VertexBuffer vb, vblightmap, vbhouse;
static GLuint tex, texlightmap, gTexUi, gTexChat, gTexTerrain, gTexHouse, gTexTestBuf;
GLuint gTexBalloon;
static unsigned char* gTexUiSurface;
static unsigned char* gTexChatSurface;
static float CLSALIGN16 uipos[4];
static float CLSALIGN16 uiscale[4] = { 200.0, 200.0, 1, 1 };
static float CLSALIGN16 chatuipos[4];
float CLSALIGN16 chatuiscale[4] = { 600.0, 200.0, 1, 1 };
float cellw = 0.5f*4, cellh = 0.5f*4;
int pcx = 50, pcy = 50;
static float resolution = 1.0f;
static float heightscale = 5.0f;
static float persistence = 0.8f;
static std::list<TerrainSegment> gTerrainSegmentList;
CLSALIGN16 float v_shadowmap[16], vInv_shadowmap[16]; // shadowmap 위한 view 행렬
CLSALIGN16 float p_shadowmap[16]; // shadowmap 위한 proj 행렬

typedef boost::function<void ()> RenCommand;

static void Render(float dt, int w, int h);
void quit(int rc);
void RsSetTextureMatrix(float* mvp);

static void SetUniMvp(float* m, float* v, float* p)
{
    return;

    CLSALIGN16 float mvpdata[16];

    glhLoadIdentityf2(mvpdata);
    glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, m);
    glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, v);
    glhMultMatrixSwappedf2_SSE_Aligned(mvpdata, p);

    glUniformMatrix4fv(gUniMvp, 1, GL_FALSE, (GLfloat*)mvpdata);

    CLSALIGN16 float mvpdata_shadowmap[16];

    glhLoadIdentityf2(mvpdata_shadowmap);
    glhMultMatrixSwappedf2_SSE_Aligned(mvpdata_shadowmap, m);
    glhMultMatrixSwappedf2_SSE_Aligned(mvpdata_shadowmap, v_shadowmap);
    glhMultMatrixSwappedf2_SSE_Aligned(mvpdata_shadowmap, p_shadowmap);

    //RsSetTextureMatrix(mvpdata_shadowmap);
}

void InitializeFramebuffers(int w, int h)
{
    rs_create_pp_program();
    rs_create_backbuffer(w, h);
}

void InitializeMainGlslProgram()
{
    rs_init_default_shader_program();
    rs_init_rocket_shader_program();
}

void GetTerrainSegmentVb(VertexBuffer& vertbuf, const SDL_Point& segIndex)
{
    static const size_t TERRAIN_SEGMENT_BUFFER_SIZE = (TERRAIN_SEG_X+2) * (TERRAIN_SEG_Y+2);

    std::list<TerrainSegment>::iterator it = std::find_if(gTerrainSegmentList.begin(), gTerrainSegmentList.end(), [segIndex](const TerrainSegment& ts) {

        return ts.segIndex.x == segIndex.x
            && ts.segIndex.y == segIndex.y;

    });

    if (it != gTerrainSegmentList.end())
    {
        vertbuf = it->vertbuf;

        // 참조 당했으니 앞으로 가져오자~
        gTerrainSegmentList.splice(gTerrainSegmentList.begin(), gTerrainSegmentList, it);
    }
    else
    {
        VertexBuffer vbterrain;
        memset(&vbterrain, 0, sizeof(VertexBuffer));

        float* pz = (float*)malloc(sizeof(float)*pcx*pcy);

        float posxOffset = cellw*(pcx-1) * segIndex.x;
        float posyOffset = cellh*(pcy-1) * segIndex.y;

        cl_get_2d_map(pz, cellw, cellh, pcx, pcy, posxOffset, posyOffset);

        create_vertex_buffer_from_heightmap_3d(
            &vbterrain,
            cellw, cellh, pcx, pcy, pz);

        free(pz);

        TerrainSegment newTs;
        newTs.segIndex = segIndex;
        newTs.vertbuf = vbterrain;

        // 참조 당했으니 앞에다가 넣어야지
        gTerrainSegmentList.push_front(newTs);

        cl_debug_output("Terrain segment (%d,%d) created. [Num=%d]\n",
            segIndex.x,
            segIndex.y,
            gTerrainSegmentList.size());

        if (gTerrainSegmentList.size() > TERRAIN_SEGMENT_BUFFER_SIZE)
        {
            TerrainSegment evictedTs = *gTerrainSegmentList.rbegin();
            gTerrainSegmentList.pop_back();

            glDeleteBuffers(1, &evictedTs.vertbuf.vbuffer);
            glDeleteVertexArrays(1, &evictedTs.vertbuf.varray);

            cl_debug_output("Terrain segment (%d,%d) evicted. [Num=%d]\n",
                evictedTs.segIndex.x,
                evictedTs.segIndex.y,
                gTerrainSegmentList.size());
        }

        vertbuf = newTs.vertbuf;
    }
}

void QueryVisibleTerrainSegments(SDL_Rect& segRange, float x, float y)
{
    // 플레이트 하나당 너비, 높이
    const float segW = cellw * (pcx-1);
    const float segH = cellh * (pcy-1);

    // (x, y) 점이 속하는 플레이트 인덱스
    const SDL_Point curSeg = { (int)(x / segW), (int)(y / segH) };

    segRange.x = curSeg.x - TERRAIN_SEG_X/2;
    segRange.y = curSeg.y - TERRAIN_SEG_Y/2;
    segRange.w = TERRAIN_SEG_X;
    segRange.h = TERRAIN_SEG_Y;
}

//typedef void (APIENTRY *GLDEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam);
GLvoid APIENTRY OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam)
{
    using namespace std;

    cout << "---------------------opengl-callback-start------------" << endl;
	cout << "message: "<< message << endl;
	cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
		cout << "OTHER";
		break;
	}
	cout << endl;

	cout << "id: " << id;
    cout << endl;

    cout << "severity: ";
	switch (severity){
	case GL_DEBUG_SEVERITY_LOW:
		cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		cout << "HIGH";
		break;
	}
	cout << endl;
	cout << "---------------------opengl-callback-end--------------" << endl;
}

RenderingModule::RenderingModule(void* data)
{
    RenderingData* renderingThreadData = (RenderingData*)data;
    state = renderingThreadData->state;
    SDL_DisplayMode mode;
    VertexData3D vd3d[] = {
        // x       y    z  u v   nx ny nz  r g b a
        {0-0.5f,0-0.5f, 0, 0,0,  0, 0, 1,  1,1,1,1},
        {1-0.5f,0-0.5f, 0, 1,0,  0, 0, 1,  1,1,1,1},
        {0-0.5f,1-0.5f, 0, 0,1,  0, 0, 1,  1,1,1,1},
        {1-0.5f,0-0.5f, 0, 1,0,  0, 0, 1,  1,1,1,1},
        {1-0.5f,1-0.5f, 0, 1,1,  0, 0, 1,  1,1,1,1},
        {0-0.5f,1-0.5f, 0, 0,1,  0, 0, 1,  1,1,1,1},
    };
    VertexData3D vd3dchar[] = {
        // x       y        z  u v   nx ny nz  r g b a
        {0-0.5f,0-0.5f+0.5, 0, 0,0,  0, 0, 1,  1,1,1,1},
        {1-0.5f,0-0.5f+0.5, 0, 1,0,  0, 0, 1,  1,1,1,1},
        {0-0.5f,1-0.5f+0.5, 0, 0,1,  0, 0, 1,  1,1,1,1},
        {1-0.5f,0-0.5f+0.5, 0, 1,0,  0, 0, 1,  1,1,1,1},
        {1-0.5f,1-0.5f+0.5, 0, 1,1,  0, 0, 1,  1,1,1,1},
        {0-0.5f,1-0.5f+0.5, 0, 0,1,  0, 0, 1,  1,1,1,1},
    };
    
    char resPath[1024];
    perf_count_prev = 0;

    SDL_Window * window = state->windows[0];

    /* Create OpenGL context */
    context = SDL_GL_CreateContext(window);
    if (!context) {
        cl_debug_output("SDL_GL_CreateContext(): %s\n", SDL_GetError());
        quit(2);
    }

    // 무조건 vsync 시도하자~
    //state->render_flags |= SDL_RENDERER_PRESENTVSYNC;

    if (state->render_flags & SDL_RENDERER_PRESENTVSYNC) {
        /* try late-swap-tearing first. If not supported, try normal vsync. */
        if (SDL_GL_SetSwapInterval(-1) == -1) {
            SDL_GL_SetSwapInterval(1);
        }
    } else {
        SDL_GL_SetSwapInterval(0);  /* disable vsync. */
    }
    extern GLboolean glewExperimental;
    glewExperimental = TRUE;
    GLenum glewErr = glewInit();
    if (GLEW_OK != glewErr) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "GLEW INIT FAILED. ABORTING...", window);

        throw std::runtime_error("GLEW FAIL");
    }

    extern bool gOpenGl43;
    if (gOpenGl43 ? !GLEW_VERSION_4_3 : !GLEW_VERSION_3_0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "UNSUPPORTED OpenGL VERSION. ABORTING...", window);

        throw std::runtime_error("OPENGL FAIL");
    }

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    GLuint unusedIds = 0;
    typedef void (GLAPIENTRY * PFNGLDEBUGMESSAGECONTROLPROC) (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint* ids, GLboolean enabled);
    PFNGLDEBUGMESSAGECONTROLPROC pglDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)wglGetProcAddress("glDebugMessageControl");
    if (pglDebugMessageControl)
    {
        pglDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, GL_TRUE);
    }
	
    typedef void (GLAPIENTRY * PFNGLDEBUGMESSAGECALLBACKPROC) (GLDEBUGPROC callback, void* userParam);
    PFNGLDEBUGMESSAGECALLBACKPROC pglDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback");
    if (pglDebugMessageCallback)
    {
        pglDebugMessageCallback(OpenGLDebugCallback, nullptr);
    }

    SDL_GetCurrentDisplayMode(0, &mode);
    cl_debug_output("Screen BPP    : %d\n", SDL_BITSPERPIXEL(mode.format));
    cl_debug_output("Swap Interval : %d\n", SDL_GL_GetSwapInterval());
    cl_debug_output("\n");
    cl_debug_output("Vendor        : %s\n", glGetString(GL_VENDOR));
    cl_debug_output("Renderer      : %s\n", glGetString(GL_RENDERER));
    cl_debug_output("Version       : %s\n", glGetString(GL_VERSION));
    cl_debug_output("\n");

    //rs_test_load_fbx_file();

    GLenum e;
    e = glGetError();

    InitializeRocket();

    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    InitializeFramebuffers(w, h);

    //SDL_GetWindowSize(state->windows[0], &sm_conf.shadowMapWidth, &sm_conf.shadowMapHeight);
    sm_conf.shadowMapWidth = 1024;
    sm_conf.shadowMapHeight = 1024;
    rs_generateShadowFBO(&sm_conf);
    rs_build_shadowmap_shader_program(&sm_conf);

    /* Set rendering settings */
    glClearColor(0.0, 1.0, 1.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);

    InitializeMainGlslProgram();

    //rs_load_fbx(&testVbBuf, &testVbBufSize, "C:\\Users\\Ben\\Documents\\test.fbx");
    GetMmoResourcePath(resPath, 1024, "test.dds");
    gTexTestBuf = load_dds(resPath);

    GetMmoResourcePath(resPath, 1024, "capsule.obj");
    load_obj(&vb, resPath);

    //GetMmoResourcePath(resPath, 1024, "cylinder-rock.obj");
    //load_obj(&vbcylinder, resPath);

    GetMmoResourcePath(resPath, 1024, "house.obj");
    load_obj(&vbhouse, resPath);

    //glLoadIdentity();
    //glFrustum(-30,30,-30,30,0.1,100);

    GetMmoResourcePath(resPath, 1024, "capsule.dds");
    tex = load_dds(resPath);

    GetMmoResourcePath(resPath, 1024, "lightmap.dds");
    texlightmap = load_dds(resPath);

    GetMmoResourcePath(resPath, 1024, "grassland.dds");
    gTexTerrain = load_dds(resPath);

    //GetMmoResourcePath(resPath, 1024, "005-Beach01.dds");
    //gTexCylinderRock = load_dds(resPath);

    gRockCylinder.objFileName = "cylinder-rock.obj";
    gRockCylinder.ddsFileName = "005-Beach01.dds";
    rs_load_simple_obj(&gRockCylinder);

    GetMmoResourcePath(resPath, 1024, "english_house_texture.dds");
    gTexHouse = load_dds(resPath);

    GetMmoResourcePath(resPath, 1024, "lightmap.obj");
    load_obj(&vblightmap, resPath);

    vbui.data = vd3d;
    vbui.len = _countof(vd3d);
    create_vertex_buffer_from_array_3d(&vbui);

    vbchar.data = vd3dchar;
    vbchar.len = _countof(vd3dchar);
    create_vertex_buffer_from_array_3d(&vbchar);

    create_vertex_buffer_pie(&vbpie, 1.0f, 10);

    uiscale[0] = 256;
    uiscale[1] = 256;

    {
        int w, h;
        SDL_GetWindowSize(state->windows[0], &w, &h);

        uipos[0] = uiscale[0]/2;
        uipos[1] = uiscale[1]/2;

        chatuipos[0] = chatuiscale[0]/2;
        chatuipos[1] = chatuiscale[1]/2;
    }

    gTexUi = CreateSigninWindowBackgroundTexture(&gTexUiSurface);
    gTexChat = CreateChatWindowBackgroundTexture(&gTexChatSurface, (int)chatuiscale[0], (int)chatuiscale[1]);

    unsigned char balloonbg[4] = { 0x2f, 0x2f, 0x2f, 0x2f };
    gTexBalloon = CreateTextureFromMemory(1, 1, balloonbg);
}

RenderingModule::~RenderingModule()
{
    // 이 미친 NVIDIA OpenGL 드라이버가 메모리 해제를 안해주고 프로그램 끝내면 크래시를 내네? ㅋㅋ
    glDeleteBuffers(1, &vb.vbuffer);
    glDeleteVertexArrays(1, &vb.varray);

    glDeleteBuffers(1, &vblightmap.vbuffer);
    glDeleteVertexArrays(1, &vblightmap.varray);

    glDeleteBuffers(1, &vbui.vbuffer);
    glDeleteVertexArrays(1, &vbui.varray);

    FinalizeRocket();

    if (context) {
        /* SDL_GL_MakeCurrent(0, NULL); *//* doesn't do anything */
        SDL_GL_DeleteContext(context);
    }
}

void RenderingModule::Tick(float dt)
{
    int w, h;

    //SDL_GL_MakeCurrent(state->windows[0], context);
    SDL_GetWindowSize(state->windows[0], &w, &h);

    Render(dt, w, h);

    //glDrawBuffer(GL_FRONT);
    //SDL_Delay(1);
    //glFinish();

    SDL_GL_SwapWindow(state->windows[0]);

    /*while(1)
    {
    Sleep(1);
    }*/


    float fs = rs_get_fullscreen_backbuffer_scale();
    const float targetScale = 1.0f;
    if (fs < targetScale)
        fs += 0.01f;
    if (fs > targetScale)
        fs = targetScale;
    rs_set_fullscreen_backbuffer_scale(fs);
}

//int SDLCALL RenderingThread(void *data)
//{
//    std::shared_ptr<RenderingModule> rtm(new RenderingModule(data));
//
//    while (gDoRenderingLoop)
//    {
//        rtm->Tick(dt);
//    }
//
//    return 0;
//}

static void RenderCylinderRock(float x, float y, float z, float* rot43, float* v, float* p)
{
    CLSALIGN16 float m[16], rot44[16];
    CLSALIGN16 float xyzw[4] = { x, y, z, 1.0f };
    CLSALIGN16 float scale[4] = { 3.0f, 3.0f, 5.0f/2, 1.0f };
    CLSALIGN16 float zaxis[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

    memcpy(rot44, rot43, sizeof(float)*4*3);

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (i == 3)
            {
                if (j < 3)
                    rot44[4*j + i] = 0;
                else
                    rot44[4*j + i] = 1;
            }
            else
            {
                rot44[4*j + i] = rot43[4*i + j];
            }
        }
    }

    glhLoadIdentityf2(m);
    glhTranslatef2_SSE_Aligned(m, xyzw);
    glhScalef2_SSE_Aligned(m, scale);
    glhMultMatrixf2_SSE_Aligned(m, rot44);
    glhRotatef2_SSE_Aligned(m, (float)M_PI, zaxis);

    rs_set_uvx(1,1,0,0);
    rs_render_triangles(gRockCylinder.texId, gRockCylinder.vb.varray, gRockCylinder.vb.len, m);
}

static void RenderHouse(float x, float y, float z, float* rot43, float* v, float* p)
{
    CLSALIGN16 float m[16], rot44[16];
    CLSALIGN16 float xyzw[4] = { x, y, z, 1.0f };
    CLSALIGN16 float scale[4] = { 3.0f, 3.0f, 5.0f/2, 1.0f };
    CLSALIGN16 float zaxis[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

    memcpy(rot44, rot43, sizeof(float)*4*3);

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (i == 3)
            {
                if (j < 3)
                    rot44[4*j + i] = 0;
                else
                    rot44[4*j + i] = 1;
            }
            else
            {
                rot44[4*j + i] = rot43[4*i + j];
            }
        }
    }

    glhLoadIdentityf2(m);
    glhTranslatef2_SSE_Aligned(m, xyzw);
    glhScalef2_SSE_Aligned(m, scale);
    glhMultMatrixf2_SSE_Aligned(m, rot44);
    glhRotatef2_SSE_Aligned(m, (float)M_PI, zaxis);

    rs_set_uvx(1,1,0,0);
    rs_render_triangles(gTexHouse, vbhouse.varray, vbhouse.len, m);
}

static void RenderTestVbBuf(float x, float y, float z, float* rot43, float* v, float* p)
{
    CLSALIGN16 float m[16], rot44[16];
    CLSALIGN16 float xyzw[4] = { x, y, z, 1.0f };
    CLSALIGN16 float scale[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    CLSALIGN16 float zaxis[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

    memset(rot44, 0, sizeof(rot44));
    rot44[0] = rot44[5] = rot44[10] = rot44[15] = 1.0f;

    //memcpy(rot44, rot43, sizeof(float)*4*3);

    glhLoadIdentityf2(m);
    glhTranslatef2_SSE_Aligned(m, xyzw);
    glhScalef2_SSE_Aligned(m, scale);
    glhMultMatrixf2_SSE_Aligned(m, rot44);
    glhRotatef2_SSE_Aligned(m, (float)M_PI, zaxis);

    rs_set_uvx(1,1,0,0);

	if (testVbBuf)
	{
		rs_render_triangles(gTexTestBuf, testVbBuf[0].varray, testVbBuf[0].len, m);
	}
}

static void RenderCapsuleMan(float x, float y, float z, float* rot43, float* v, float* p)
{
    CLSALIGN16 float m[16], rot44[16];
    CLSALIGN16 float xyzw[4] = { x, y, z, 1.0f };
    CLSALIGN16 float scale[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    CLSALIGN16 float zaxis[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

    memcpy(rot44, rot43, sizeof(float)*4*3);

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            if (i == 3)
            {
                if (j < 3)
                    rot44[4*j + i] = 0;
                else
                    rot44[4*j + i] = 1;
            }
            else
            {
                rot44[4*j + i] = rot43[4*i + j];
            }
        }
    }

    /*rot44[12] = 0.0f;
    rot44[13] = 0.0f;
    rot44[14] = 0.0f;
    rot44[15] = 1.0f;*/

    glhLoadIdentityf2(m);
    glhTranslatef2_SSE_Aligned(m, xyzw);
    glhScalef2_SSE_Aligned(m, scale);
    glhMultMatrixf2_SSE_Aligned(m, rot44);
    glhRotatef2_SSE_Aligned(m, (float)M_PI, zaxis);

    float uvx[9];
    glhLoadIdentity3f2(uvx);
    glUniformMatrix3fv(gUniUvx, 1, GL_FALSE, (GLfloat*)uvx);
    SetUniMvp(m, v, p);

    glBindTexture(GL_TEXTURE_2D, tex);
    glBindVertexArray(vb.varray);
    glDrawArrays(GL_TRIANGLES, 0, vb.len);
    glBindVertexArray(0);
}

static void RenderRpgMan(const cl_visible_entity& e, float* v, float* p, float* vInv)
{
    cl_tex_info cti;
    cl_get_tex_info(e.resource_name, &cti);

    const char* sprite_name = cl_sprite_name(e.resource_name);
    GLuint texName = rs_get_texture_from_png_file(e.resource_name, sprite_name);
    rs_get_texture_size(e.resource_name, &cti.w, &cti.h);

    if (cti.w == 0 || cti.h == 0 || cti.umax == 0 || cti.vmax == 0 || cti.sprite_count == 0)
        return;

    CLSALIGN16 float m[16];
    const float imageRatio = ((float)cti.w/cti.umax) / (cti.h/cti.vmax);
    CLSALIGN16 float t[4] = { (float)e.x, (float)e.y, (float)e.z, 1.0f };
    CLSALIGN16 float s[4] = { cti.scale * imageRatio, cti.scale, 1.0f, 0.0f };

    CLSALIGN16 float vInvNoTrans[16];
    memset(vInvNoTrans, 0, sizeof(vInvNoTrans));
    memcpy(vInvNoTrans, vInv, sizeof(vInvNoTrans));
    memcpy(vInvNoTrans + 12, t, sizeof(t));

    glhLoadIdentityf2(m);
    glhScalef2_SSE_Aligned(m, s);
    glhMultMatrixSwappedf2_SSE_Aligned(m, vInvNoTrans);

    const float su = 1.0f/cti.umax;
    const float sv = 1.0f/cti.vmax;

    /*
    float uvx[9] = {
        su  , 0.0f, 0.0f,
        0.0f,   sv, 0.0f,
        su * e.uindex, sv * e.vindex, 1.0f
    };
    glUniformMatrix3fv(gUniUvx, 1, GL_FALSE, (GLfloat*)uvx);
    SetUniMvp(m, v, p);
    */
    rs_set_uvx(su, sv, su * e.uindex, sv * e.vindex);
    
    //glUniform1f(gUniAlpha, cl_get_hp_ratio(e.id));
    rs_set_alpha(cl_get_hp_ratio(e.id));

    /*
    glBindTexture(GL_TEXTURE_2D, texName);
    glBindVertexArray(vbchar.varray);
    glDrawArrays(GL_TRIANGLES, 0, vbchar.len);
    glBindVertexArray(0);
    */
    rs_render_triangles(texName, vbchar.varray, vbchar.len, m);

    //glUniform1f(gUniAlpha, 1.0f);
    rs_set_alpha(1.0f);

    //glhLoadIdentity3f2(uvx);
    //glUniformMatrix3fv(gUniUvx, 1, GL_FALSE, (GLfloat*)uvx);
    rs_set_uvx(1, 1, 0, 0);
}

static SimpleStaticObject GetStaticObjectResource(const cl_static_object_info& csoi)
{
    SimpleStaticObject sso;
    memset(&sso, 0, sizeof(sso));
    
    if (csoi.name == nullptr || csoi.dds == nullptr)
    {
        assert(false);
        return sso;
    }
    
    sso.ddsFileName = csoi.dds;
    sso.objFileName = csoi.name;
    rs_load_simple_obj(&sso);

    return sso;
}

static void RenderStaticObject(const cl_visible_entity& e, float* v, float* p, float* vInv)
{
    cl_static_object_info csoi;
    cl_get_static_object_info(e.resource_name, &csoi);

    CLSALIGN16 float m[16];
    CLSALIGN16 float xyzw[4] = { (float)e.x, (float)e.y, (float)e.z, 1.0f };
    CLSALIGN16 float scale[4] = { csoi.scale, csoi.scale, csoi.scale, 1.0f };
    CLSALIGN16 float zaxis[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

    glhLoadIdentityf2(m);
    glhTranslatef2_SSE_Aligned(m, xyzw);
    glhScalef2_SSE_Aligned(m, scale);
    glhRotatef2_SSE_Aligned(m, (float)M_PI, zaxis);

    SimpleStaticObject sso = GetStaticObjectResource(csoi);

    rs_set_uvx(1,1,0,0);
    rs_render_triangles(sso.texId, sso.vb.varray, sso.vb.len, m);
}

static void RenderEntity(const cl_visible_entity& e, float* v, float* p, float* vInv)
{
    if (e.is_static_object)
    {
        RenderStaticObject(e, v, p, vInv);
    }
    else
    {
        RenderRpgMan(e, v, p, vInv);
    }
}

static float gRockX = 0;
    
static void RenderShadowedShapes(float dt, int w, int h, float* v, float* p, float* vInv)
{
    rs_set_view_matrix(v);
    rs_set_proj_matrix(p);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    float CLSALIGN16 m[16]; //, v[16], vInv[16], p[16], mvpdata[16];

    // Level //////////////////////////////////////////////////////

    glhLoadIdentityf2(m);
    /*CLSALIGN16 float tlevel[4] = {
    -pcx * cellw / 2,
    -pcy * cellh / 2,
    0,
    1.0f
    };*/
    CLSALIGN16 float tlevel[4] = {
        0,
        0,
        0,
        1.0f
    };

    glhTranslatef2_SSE_Aligned(m, tlevel);

    //glhMultMatrixSwappedf2_SSE_Aligned

    

    SetUniMvp(m, v, p);

    

    extern float sunTheta;
    CLSALIGN16 float dirLight[4] = {
        1-2*sinf(sunTheta/2),
        -1+2*sinf(sunTheta/2),
        sinf(sunTheta),
        0
    };
    //glUniform4fv(gUniDirLight, 1, (GLfloat*)dirLight);
    rs_set_light_direction(dirLight);
       

    rs_set_uvx(pcx*cellw/4, pcx*cellh/4, 0, 0);

    

    double playerx = 0, playery = 0, playerz = 0;
    bool trackPlayer = (cl_player_pos(&playerx, &playery) == CEC_OKAY)
        && (cl_player_pos_z(&playerz) == CEC_OKAY);

    //glBindTexture(GL_TEXTURE_2D, gTexTerrain);

    SDL_Rect segRange;
    QueryVisibleTerrainSegments(segRange, (float)playerx, (float)playery);

    //glPushAttrib(GL_POLYGON_BIT);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    for (int i = segRange.y; i < segRange.y+segRange.h; ++i)
    {
        for (int j = segRange.x; j < segRange.x+segRange.w; ++j)
        {
            CLSALIGN16 float tlevel[4] = {
                j * cellw * (pcx-1),
                i * cellh * (pcy-1),
                0,
                1.0f
            };

            glhLoadIdentityf2(m);
            glhTranslatef2_SSE_Aligned(m, tlevel);

            //glhMultMatrixSwappedf2_SSE_Aligned

            //SetUniMvp(m, v, p);

            VertexBuffer vertbuf;

            SDL_Point segIndex = { j, i };
            GetTerrainSegmentVb(vertbuf, segIndex);

            rs_render_triangles(gTexTerrain, vertbuf.varray, vertbuf.len, m);

            /*
            glBindVertexArray(vertbuf.varray);
            glDrawArrays(GL_TRIANGLES, 0, vertbuf.len);
            glBindVertexArray(0);
            */
        }
    }
    //glPopAttrib();

    float uvxdata[9];

    glhLoadIdentity3f2(uvxdata);
    //glUniformMatrix3fv(gUniUvx, 1, GL_FALSE, (GLfloat*)uvxdata);

    

    // Entities //////////////////////////////////////////////////////

    // 캡슐맨
    //float ghostpos[3];
    //cl_get_logicworld_ghost_pos(ghostpos);
    //RenderCapsuleMan(ghostpos[0], ghostpos[1], ghostpos[2], player.rot, v, p);

    // 실린더 바위
    RenderCylinderRock(gRockX, 0, 0, player.rot, v, p);
    
    /*
    RenderCylinderRock(0, 10, 0, player.rot, v, p);
    RenderCylinderRock(10, 0, 0, player.rot, v, p);
    */

    

    // 좋은 집
    RenderHouse(-10, 25, 0, player.rot, v, p);

    // FBX 테스트
    RenderTestVbBuf(0, 0, 5, player.rot, v, p);


    // 캡슐맨 십! 자! 가!
    for (INT i = -20; i < 20; ++i)
    {
        //RenderCapsuleMan(0.0f, 0.0f + 2 * i, 3, player.rot, v, p);
    }

    for (INT i = -20; i < 20; ++i)
    {
        //RenderCapsuleMan(0.0f + 2 * i, 0.0f, 3, player.rot, v, p);
    }

    int ec = cl_visible_entity_count();
    if (ec > 0)
    {
        std::vector<cl_visible_entity> cve(ec);
        int realCount = cl_visible_entities(&cve[0], ec);
        cve.resize(realCount);

        std::sort(cve.begin(), cve.end(), [](const cl_visible_entity& e1, const cl_visible_entity& e2) {
            return e1.y > e2.y;
        });

        for (cl_visible_entity& e : cve)
        {
            RenderEntity(e, v, p, vInv);
            
            if (cl_entity_nameplate_visible(e.id))
            {
                const char* entityName = cl_entity_name(e.id);
                wchar_t* entityName2 = SDL_create_wide_str(entityName);

                extern GLuint gTexBalloon;
                extern GLint gUniMvp;
                extern VertexBuffer vbui, vbpie;

                BalloonConfig bc;
                bc.ox = (float)e.x;
                bc.oy = (float)e.y;
                bc.oz = (float)e.z;
                bc.bwidth = 5.0f;
                bc.bheight = 10.0f;
                bc.r = 1.5f;
                bc.v = v;
                bc.p = p;
                bc.alignZ = 1;
                bc.text = entityName2;
                bc.vInv = vInv;
                bc.gUniMvp = gUniMvp;
                bc.vbpie = vbpie;
                bc.vbui = vbui;
                bc.gTexBalloon = gTexBalloon;
                
                //RenderBalloon(&bc);

                SDL_free(entityName2);
            }
        }
    }
}
                
static void UpdateChatWindow()
{
    extern Rocket::Core::Element* gChatLogField;
    if (gChatLogField)
    {
        std::wstring lines;
        
        for (const std::wstring& chatLogLine : gChatLog)
        {
            lines += chatLogLine;
            lines += L"<br/>";
        }

        gChatLogField->SetInnerRML(to_utf8(lines).c_str());
    }
}

static void RenderUILayer(float dt, int w, int h, float* v, float* p)
{
    float CLSALIGN16 m[16]; //, v[16], vInv[16], p[16], mvpdata[16];

    // UI Layer ///////////////////////////////

    if (gUiMode)
    {
        CLSALIGN16 const float uiviewscale[4] = { 2.0f / w, 2.0f / h, 1.0f, 1.0f };
        CLSALIGN16 const float uiviewtrans[4] = { -w/2.0f, -h/2.0f, 0, 0 };

        glhLoadIdentityf2(m);
        glhTranslatef2_SSE_Aligned(m, uipos);
        glhScalef2_SSE_Aligned(m, uiscale);

        glhLoadIdentityf2(v);
        glhScalef2_SSE_Aligned(v, uiviewscale);
        glhTranslatef2_SSE_Aligned(v, uiviewtrans);

        glhLoadIdentityf2(p);

        // modify texture according to user text input
        if (InterlockedExchange(&gRedrawCurrentIme, 0))
        {
            wchar_t userName[1024] = {0};
            wchar_t worldName[1024] = {0};
            wcscat(userName, gUserNameText);
            wcscat(userName, gCompositingUserNameText);
            wcscat(worldName, gWorldNameText);
            wcscat(worldName, gCompositingWorldNameText);

            RedrawSigninWindow(gTexUiSurface, gTexUi, userName, worldName);
        }

        SetUniMvp(m, v, p);
        glBindTexture(GL_TEXTURE_2D, gTexUi);

        glBindVertexArray(vbui.varray);
        glDrawArrays(GL_TRIANGLES, 0, vbui.len);
        glBindVertexArray(0);

    }

    if (true /*gChatMode*/)
    {
        /*CLSALIGN16 float uiviewscale[4] = { 2.0f / w, 2.0f / h, 1.0f, 1.0f };
        CLSALIGN16 float uiviewtrans[4] = { -w/2.0f, -h/2.0f, 0, 0 };

        glhLoadIdentityf2(m);
        glhTranslatef2_SSE_Aligned(m, chatuipos);
        glhScalef2_SSE_Aligned(m, chatuiscale);

        glhLoadIdentityf2(v);
        glhScalef2_SSE_Aligned(v, uiviewscale);
        glhTranslatef2_SSE_Aligned(v, uiviewtrans);

        glhLoadIdentityf2(p);*/

        // modify texture according to user text input
        if (InterlockedExchange(&gRedrawCurrentIme, 0))
        {
            /*wchar_t chatLine[1024] = {0};
            wcscat(chatLine, gChatLineText);
            wcscat(chatLine, gCompositingChatLineText);

            std::wstring lines;
            for (const std::wstring& chatLogLine : gChatLog)
            {
                lines += chatLogLine;
                lines += L'\n';
            }*/

            //RedrawChatWindow(gTexChatSurface, gTexChat, lines.c_str(), chatLine, TRUE);

            // Rocket!
            UpdateChatWindow();
        }

        /*SetUniMvp(m, v, p);
        glBindTexture(GL_TEXTURE_2D, gTexChat);
        glBindVertexArray(vbui.varray);
        glDrawArrays(GL_TRIANGLES, 0, vbui.len);
        glBindVertexArray(0);*/
    }


    // Screen Texts ////////////////////////////////////////
    RenderScreenTexts(w, h, dt);
}

static void Render(float dt, int w, int h)
{
    double playerx = 0, playery = 0, playerz = 0;
    bool trackPlayer = (cl_player_pos(&playerx, &playery) == CEC_OKAY)
        && (cl_player_pos_z(&playerz) == CEC_OKAY);

    float zPos = 0;
    if (trackPlayer)
    {
        cl_get_2d_map_query_height(&zPos, (float)playerx, (float)playery);
    }

    const float CLSALIGN16 camtrackplayertrans[] = {
        trackPlayer ? (float)-playerx : 0,
        trackPlayer ? (float)-playery : 0,
        trackPlayer ? -zPos : 0,
        0
    };

    const float CLSALIGN16 camtrackplayertrans_shadowmap[] = { 0, 0, 0, 0 };

    CLSALIGN16 float v[16], vInv[16];
    
    calculate_view_matrix(v, vInv, camyaw, campitch, camdist, camtrackplayertrans);
    calculate_view_matrix(v_shadowmap, vInv_shadowmap, float(-M_PI/4), float(M_PI/4), 150.0f, camtrackplayertrans_shadowmap);
    rs_set_shadow_view_matrix(v_shadowmap);

    CLSALIGN16 float p[16];
    glhLoadIdentityf2(p);
    glhPerspectivef2(p, 45, (float)w/h, 1.0f, 10000.0f);


    glhLoadIdentityf2(p_shadowmap);
    glhPerspectivef2(p_shadowmap, 45, (float)w/h, 10.0f, 40000.0f);
    rs_set_shadow_proj_matrix(p_shadowmap);
    

    // 그림자 렌더링 프로그램 ㄱㄱ
    rs_use_default_program();
    //glUseProgram(gMainProgram);
    
    rs_set_alpha(1.0f);
    //glUniform1f(gUniAlpha, 1.0f);

    /////////////////////////////////////////////////////////////////////////////////////

    //First step: Render from the light POV to a FBO, story depth values only
    glBindFramebuffer(GL_FRAMEBUFFER, sm_conf.fboId);	//Rendering offscreen

    

    //Using the fixed pipeline to render to the depthbuffer
    //glUseProgramObjectARB(sm_conf.);

    // In the case we render the shadowmap to a higher resolution, the viewport must be modified accordingly.
    glViewport(0, 0, sm_conf.shadowMapWidth, sm_conf.shadowMapHeight);

    // Clear previous frame values
    glClear(GL_DEPTH_BUFFER_BIT);

    //Disable color rendering, we only want to write to the Z-Buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        
    // 생성된 그림자 텍스쳐에다가 그리기
    // Culling switching, rendering only backface, this is done to avoid self-shadowing
    glCullFace(GL_FRONT);
    RenderShadowedShapes(dt, w, h, v_shadowmap, p_shadowmap, vInv_shadowmap);

    // 그림자 렌더링 결과 텍스쳐 7번에 저장해 두었으니...
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, sm_conf.depthTextureId);
    glActiveTexture(GL_TEXTURE0);


    // 진짜 렌더링 시작~~~
    rs_start_backbuffer_rendering();

    glCullFace(GL_BACK);
    glViewport(0, 0, w, h);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    

    RenderShadowedShapes(dt, w, h, v, p, vInv);


    //RenderUILayer(dt, w, h, v, p);
    if (InterlockedExchange(&gRedrawCurrentIme, 0))
    {
        UpdateChatWindow();
    }

    rs_render_fullscreen_backbuffer();

    

    // Rocket (UI)
    rs_use_rocket_program();
    
    UpdateRenderRocket(w, h);

    // Client Logic /////////////////////////////////////
    
    if (gFire)
    {
        cl_spawn_fireball(cl_player_id());
    }

    cl_frame_move(dt);

    std::string random_chat;
    static const int random_chat_max_len = 10;
    int random_chat_len = rand() % random_chat_max_len;
    std::random_device rd;
    std::mt19937 g(rd());
    for (int i = 0; i < random_chat_len; ++i)
    {
        random_chat += 'a' + (i % 26);
    }
    std::shuffle(random_chat.begin(), random_chat.end(), g);
    //cl_player_chat(random_chat.c_str());

    static float gRockXVel = 10;
    gRockX += gRockXVel * dt;
    if (gRockX > 10.0f)
    {
        gRockXVel = -fabs(gRockXVel);
    }
    else if (gRockX < -10.0f)
    {
        gRockXVel = fabs(gRockXVel);
    }
}
