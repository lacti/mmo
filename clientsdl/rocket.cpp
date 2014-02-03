#include "stdafx.h"
#include "rocket.h"
#include "EventInstancer.h"
#include "EventManager.h"
#include "EventHandlerBackground.h"
#include "serverlisthandler.h"
#include "serverlistdatasource.h"
#include "debugoverlayhandler.h"
#include "escmenuhandler.h"
#include "chathandler.h"
#include "../rendering_service/rocketshaderprogram.h"
#include "../mmobase/matrix.h"

ShellRenderInterfaceOpenGL gRenderInterfaceOpenGL;
ShellSystemInterface gSystemInterface;
Rocket::Core::Context* gContext = NULL;
static const int ROCKET_WIDTH = 1024;
static const int ROCKET_HEIGHT = 768;
static std::map<SDL_Keycode, Rocket::Core::Input::KeyIdentifier> SDLK_keymap;

void InitSDLtoRocketKeymap()
{
    using namespace Rocket::Core::Input;

    SDLK_keymap[SDLK_UNKNOWN] = KI_UNKNOWN;
    SDLK_keymap[SDLK_SPACE] = KI_SPACE;
    SDLK_keymap[SDLK_0] = KI_0;
    SDLK_keymap[SDLK_1] = KI_1;
    SDLK_keymap[SDLK_2] = KI_2;
    SDLK_keymap[SDLK_3] = KI_3;
    SDLK_keymap[SDLK_4] = KI_4;
    SDLK_keymap[SDLK_5] = KI_5;
    SDLK_keymap[SDLK_6] = KI_6;
    SDLK_keymap[SDLK_7] = KI_7;
    SDLK_keymap[SDLK_8] = KI_8;
    SDLK_keymap[SDLK_9] = KI_9;
    SDLK_keymap[SDLK_a] = KI_A;
    SDLK_keymap[SDLK_b] = KI_B;
    SDLK_keymap[SDLK_c] = KI_C;
    SDLK_keymap[SDLK_d] = KI_D;
    SDLK_keymap[SDLK_e] = KI_E;
    SDLK_keymap[SDLK_f] = KI_F;
    SDLK_keymap[SDLK_g] = KI_G;
    SDLK_keymap[SDLK_h] = KI_H;
    SDLK_keymap[SDLK_i] = KI_I;
    SDLK_keymap[SDLK_j] = KI_J;
    SDLK_keymap[SDLK_k] = KI_K;
    SDLK_keymap[SDLK_l] = KI_L;
    SDLK_keymap[SDLK_m] = KI_M;
    SDLK_keymap[SDLK_n] = KI_N;
    SDLK_keymap[SDLK_o] = KI_O;
    SDLK_keymap[SDLK_p] = KI_P;
    SDLK_keymap[SDLK_q] = KI_Q;
    SDLK_keymap[SDLK_r] = KI_R;
    SDLK_keymap[SDLK_s] = KI_S;
    SDLK_keymap[SDLK_t] = KI_T;
    SDLK_keymap[SDLK_u] = KI_U;
    SDLK_keymap[SDLK_v] = KI_V;
    SDLK_keymap[SDLK_w] = KI_W;
    SDLK_keymap[SDLK_x] = KI_X;
    SDLK_keymap[SDLK_y] = KI_Y;
    SDLK_keymap[SDLK_z] = KI_Z;
    SDLK_keymap[SDLK_SEMICOLON] = KI_OEM_1;
    SDLK_keymap[SDLK_PLUS] = KI_OEM_PLUS;
    SDLK_keymap[SDLK_COMMA] = KI_OEM_COMMA;
    SDLK_keymap[SDLK_MINUS] = KI_OEM_MINUS;
    SDLK_keymap[SDLK_PERIOD] = KI_OEM_PERIOD;
    SDLK_keymap[SDLK_SLASH] = KI_OEM_2;
    SDLK_keymap[SDLK_BACKQUOTE] = KI_OEM_3;
    SDLK_keymap[SDLK_LEFTBRACKET] = KI_OEM_4;
    SDLK_keymap[SDLK_BACKSLASH] = KI_OEM_5;
    SDLK_keymap[SDLK_RIGHTBRACKET] = KI_OEM_6;
    SDLK_keymap[SDLK_QUOTEDBL] = KI_OEM_7;
    SDLK_keymap[SDLK_KP_ENTER] = KI_NUMPADENTER;
    SDLK_keymap[SDLK_KP_MULTIPLY] = KI_MULTIPLY;
    SDLK_keymap[SDLK_KP_PLUS] = KI_ADD;
    SDLK_keymap[SDLK_KP_MINUS] = KI_SUBTRACT;
    SDLK_keymap[SDLK_KP_PERIOD] = KI_DECIMAL;
    SDLK_keymap[SDLK_KP_DIVIDE] = KI_DIVIDE;
    SDLK_keymap[SDLK_KP_EQUALS] = KI_OEM_NEC_EQUAL;
    SDLK_keymap[SDLK_BACKSPACE] = KI_BACK;
    SDLK_keymap[SDLK_TAB] = KI_TAB;
    SDLK_keymap[SDLK_CLEAR] = KI_CLEAR;
    SDLK_keymap[SDLK_RETURN] = KI_RETURN;
    SDLK_keymap[SDLK_PAUSE] = KI_PAUSE;
    SDLK_keymap[SDLK_CAPSLOCK] = KI_CAPITAL;
    SDLK_keymap[SDLK_PAGEUP] = KI_PRIOR;
    SDLK_keymap[SDLK_PAGEDOWN] = KI_NEXT;
    SDLK_keymap[SDLK_END] = KI_END;
    SDLK_keymap[SDLK_HOME] = KI_HOME;
    SDLK_keymap[SDLK_LEFT] = KI_LEFT;
    SDLK_keymap[SDLK_UP] = KI_UP;
    SDLK_keymap[SDLK_RIGHT] = KI_RIGHT;
    SDLK_keymap[SDLK_DOWN] = KI_DOWN;
    SDLK_keymap[SDLK_INSERT] = KI_INSERT;
    SDLK_keymap[SDLK_DELETE] = KI_DELETE;
    SDLK_keymap[SDLK_HELP] = KI_HELP;
    SDLK_keymap[SDLK_F1] = KI_F1;
    SDLK_keymap[SDLK_F2] = KI_F2;
    SDLK_keymap[SDLK_F3] = KI_F3;
    SDLK_keymap[SDLK_F4] = KI_F4;
    SDLK_keymap[SDLK_F5] = KI_F5;
    SDLK_keymap[SDLK_F6] = KI_F6;
    SDLK_keymap[SDLK_F7] = KI_F7;
    SDLK_keymap[SDLK_F8] = KI_F8;
    SDLK_keymap[SDLK_F9] = KI_F9;
    SDLK_keymap[SDLK_F10] = KI_F10;
    SDLK_keymap[SDLK_F11] = KI_F11;
    SDLK_keymap[SDLK_F12] = KI_F12;
    SDLK_keymap[SDLK_F13] = KI_F13;
    SDLK_keymap[SDLK_F14] = KI_F14;
    SDLK_keymap[SDLK_F15] = KI_F15;
    SDLK_keymap[SDLK_LSHIFT] = KI_LSHIFT;
    SDLK_keymap[SDLK_RSHIFT] = KI_RSHIFT;
    SDLK_keymap[SDLK_LCTRL] = KI_LCONTROL;
    SDLK_keymap[SDLK_RCTRL] = KI_RCONTROL;
    SDLK_keymap[SDLK_LALT] = KI_LMENU;
    SDLK_keymap[SDLK_RALT] = KI_RMENU;
}


void InitializeRocket()
{
    InitSDLtoRocketKeymap();

    Rocket::Core::SetRenderInterface(&gRenderInterfaceOpenGL);
    gRenderInterfaceOpenGL.SetViewport(ROCKET_WIDTH, ROCKET_HEIGHT);

    Rocket::Core::SetSystemInterface(&gSystemInterface);

    Rocket::Core::Initialise();
    // Initialise the Rocket Controls library.
    Rocket::Controls::Initialise();

    // Create the main Rocket context and set it on the shell's input layer.
    gContext = Rocket::Core::CreateContext("main", Rocket::Core::Vector2i(640, 480));
    
    if (gContext == NULL)
    {
        Rocket::Core::Shutdown();
        Shell::Shutdown();
        return;
    }

    // Initialise the Rocket debugger.
    Rocket::Debugger::Initialise(gContext);
    Input::SetContext(gContext);

    // Load the font faces required for Invaders.
    Shell::LoadFonts();

    /*
    char path[1024];
    GetMmoResourcePath(path, 1024, "background.rml");
    Rocket::Core::ElementDocument* document = gContext->LoadDocument(path);
    document->Focus();
    document->Show();
    document->RemoveReference();
    */

    EventInstancer* event_instancer = new EventInstancer();
    Rocket::Core::Factory::RegisterEventListenerInstancer(event_instancer);
    event_instancer->RemoveReference();

    ServerListDataSource::Initialize();

    EventManager::RegisterEventHandler("background", new EventHandlerBackground());
    EventManager::RegisterEventHandler("serverlist", new ServerListHandler());
    EventManager::RegisterEventHandler("debugoverlay", new DebugOverlayHandler());
    EventManager::RegisterEventHandler("escmenu", new EscMenuHandler());
    EventManager::RegisterEventHandler("chat", new ChatHandler());

    // 가장 마지막에 로드되는 창에 이벤트 핸들링 우선순위가 간다. (LoadWindow 코드 참고)
    EventManager::LoadWindow("debugoverlay");

    // 명령행에서 로그인 인자를 받은 경우라면 로그인창 띄우지 않아야지
    extern bool gFastLogin;
    if (!gFastLogin)
    {
        //bool loadSuccess = EventManager::LoadWindow("background");
        //assert(loadSuccess);
    }
}

void UpdateRenderRocket(int w, int h)
{
    gRenderInterfaceOpenGL.SetViewport(w, h);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    //glUseProgram(0); // --> 기본 셰이더 쓰자 (default shader)

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glPushAttrib(GL_DEPTH_BUFFER_BIT);
    {
        glDisable(GL_DEPTH_TEST);
        //glClearColor(1,1,1,1);
        // Set up the GL state.
        //glClearColor(0, 0, 0, 1);
        
        //*****//
        //glEnableClientState(GL_VERTEX_ARRAY);
        //glEnableClientState(GL_COLOR_ARRAY);
        //*****//

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //*****//
        //glMatrixMode(GL_PROJECTION);
        //glLoadIdentity();
        //glOrtho(0, w, h, 0, -1, 1);
        CLSALIGN16 float proj[16];
        glhOrthof2(proj, 0, (float)w, (float)h, 0, -1.0f, 1.0f);
        rs_set_rocket_proj_matrix(proj);

        //glMatrixMode(GL_MODELVIEW);
        //glLoadIdentity();
        //*****//
        CLSALIGN16 float view[16];
        glhLoadIdentityf2(view);
        rs_set_rocket_view_matrix(view);

        gContext->Render();
    }
    //glPopAttrib();
}

void SetUserList(std::vector<std::string> list)
{
    auto doc = gContext->GetDocument("background");
    if (doc)
    {
        Rocket::Core::Element* elm = doc->GetElementById("user_list");
        if (elm)
        {
            static int COUNTER = 0;

            Rocket::Core::Element* new_elm = new Rocket::Core::Element("button");
            new_elm->SetAttribute("onmousedown", "messagebox");
            new_elm->SetInnerRML(Rocket::Core::String(128, "Btn %d", ++COUNTER).CString());
            elm->AppendChild(new_elm);
        }
    }
}

void FinalizeRocket()
{
    // Shutdown Rocket.

    ServerListDataSource::Shutdown();

    gContext->RemoveReference();
    Rocket::Core::Shutdown();

    Shell::CloseWindow();
    Shell::Shutdown();
}

Rocket::Core::Input::KeyIdentifier TranslateSDLKey(SDL_Keycode key)
{
    auto it = SDLK_keymap.find(key);

    return (it != SDLK_keymap.end()) ? it->second : Rocket::Core::Input::KI_UNKNOWN;
}
