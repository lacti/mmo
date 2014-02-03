#include "stdafx.h"
#include "common.h"
#include "renderingmodule.h"
#include "../mmobase/matrix.h"
#include "../mmobase/basicmath.h"
#include "../rendering_service/font.h"
#include "../rendering_service/texture.h"
#include "object.h"
#include "keycontrol.h"
#include "screentext.h"
#include "audio.h"
#include "icon.c"
#include <SDL_syswm.h>
#include "../clientrocket/InputWin32.h"
#include "rocket.h"
#include "EventManager.h"
#include "serverlistdatasource.h"

int SDLCALL PhysicsThread(void *data);
int done = 0;

void RequestCloseApp()
{
    done = 1;
}

int CHAT_LOG_LINES = 6;

int fsaa = 0;
int accel = -1;
CLSALIGN16 Object player;
BOOL gUiMode = TRUE;
BOOL gTrackballView = FALSE;
BOOL gChatMode = FALSE;
unsigned long __declspec(align(4)) gRedrawCurrentIme;
wchar_t gUserNameText[1024] = {0};
wchar_t gCompositingUserNameText[32] = {0};
wchar_t gWorldNameText[1024] = {0};
wchar_t gCompositingWorldNameText[32] = {0};
wchar_t gChatLineText[256];
wchar_t gCompositingChatLineText[32] = {0};
bool gFastLogin = false;
bool gOpenGl43 = false;
KeyControlVector kcv;
typedef std::vector<SDL_Keycode> KeyVector; // SDLK_LEFT 따위가 들어간다
KeyVector gKeyVector;
float sunTheta = (float)(M_PI/4);
float camdist = 25.0f;
float campitch = (float)(M_PI / 4.0f /*+ M_PI / 8.0f*/);
//float campitch = 0;
float camyaw = 0;
typedef boost::circular_buffer<std::wstring> ChatLogBuffer;
ChatLogBuffer gChatLog(CHAT_LOG_LINES); // 채팅 로그

static CommonState *state;
static int cursor;
static int selection_len;
static wchar_t* gImeText = gUserNameText;
static wchar_t* gCompositingText = gCompositingUserNameText;

void SetPhyPlayerPosition(float x, float y, float z);
int gFire = 0;

extern Rocket::Core::Context* gContext;

void SetPhyPlayerLinearVelZero();

typedef boost::function<void ()> RenCommand;

static void SetUiMode(BOOL b)
{
    gUiMode = b;

    if (gTrackballView)
    {
        SDL_SetRelativeMouseMode(gUiMode ? SDL_FALSE : SDL_TRUE);
        SDL_SetWindowGrab(state->windows[0], gUiMode ? SDL_FALSE : SDL_TRUE);
        SDL_ShowCursor(gUiMode ? SDL_ENABLE : SDL_DISABLE);
    }

    gUiMode ? SDL_StartTextInput() : SDL_StopTextInput();
}

static void ToggleUiMode()
{
    SetUiMode(!gUiMode);
}

/* Call this instead of exit(), so we can clean up SDL: atexit() is evil. */
void quit(int rc)
{
    CommonQuit(state);
    exit(rc);
}

static void UpdateDebugPrint(float dt)
{
    extern Rocket::Core::Element* gFpsField;
    if (gFpsField)
    {
        Rocket::Core::String s;
        s.FormatString(10, "%2.2f", 1.0f/dt);
        gFpsField->SetInnerRML(s);
    }

    extern Rocket::Core::Element* gPos0Field;
    if (gPos0Field)
    {
        float p[3];
        cl_get_player_pos0(p);
        Rocket::Core::String s;
        s.FormatString(40, "[%2.2f, %2.2f, %2.2f]", p[0], p[1], p[2]);
        gPos0Field->SetInnerRML(s);
    }

    extern Rocket::Core::Element* gPos1Field;
    if (gPos1Field)
    {
        float p[3], dp[3];
        cl_get_player_pos1(p);
        cl_get_player_pos0d(dp);
        Rocket::Core::String s;
        s.FormatString(80, "[%2.2f, %2.2f, %2.2f] / d [%2.2f, %2.2f, %2.2f]",
            p[0], p[1], p[2], dp[0], dp[1], dp[2]);
        gPos1Field->SetInnerRML(s);
    }
}

static void SendEntitiyCommandMoveByKey(SDL_Keycode key)
{
    switch (key)
    {
    case SDLK_LEFT:
        cl_entity_command(CAC_ENTITY_MOVE_LEFT, cl_player_id());
        break;
    case SDLK_RIGHT:
        cl_entity_command(CAC_ENTITY_MOVE_RIGHT, cl_player_id());
        break;
    case SDLK_UP:
        cl_entity_command(CAC_ENTITY_MOVE_UP, cl_player_id());
        break;
    case SDLK_DOWN:
        cl_entity_command(CAC_ENTITY_MOVE_DOWN, cl_player_id());
        break;
    }
}

void SetPhyPlayerLinearVel(const float* vx, const float* vy, const float* vz);
void SetPhyPlayerForce(float fx, float fy, float fz);
void SetPhyPlayerLinearVelDelta(float dvx, float dvy, float dvz);

static void SendPhyCommandMoveByKey(SDL_Keycode key)
{
    static const float phySpeedPos = 30.0f;
    static const float phySpeedNeg = -phySpeedPos;
    static const float ZERO = 0.0f;
    static const float phyJumpSpeed = 100.0f;

    /*
    cl_set_logicworld_ghost_left(false);
    cl_set_logicworld_ghost_right(false);
    cl_set_logicworld_ghost_up(false);
    cl_set_logicworld_ghost_down(false);
    cl_set_logicworld_ghost_jump(false);
    */

    switch (key)
    {
    case SDLK_LEFT:
        cl_set_logicworld_ghost_left(false);
        cl_set_logicworld_ghost_right(false);
        cl_set_logicworld_ghost_up(false);
        cl_set_logicworld_ghost_down(false);

        cl_set_logicworld_ghost_left(true);
        //SetPhyPlayerLinearVel(&phySpeedNeg, nullptr, nullptr);
        break;
    case SDLK_RIGHT:
        cl_set_logicworld_ghost_left(false);
        cl_set_logicworld_ghost_right(false);
        cl_set_logicworld_ghost_up(false);
        cl_set_logicworld_ghost_down(false);

        cl_set_logicworld_ghost_right(true);
        //SetPhyPlayerLinearVel(&phySpeedPos, nullptr, nullptr);
        break;
    case SDLK_UP:
        cl_set_logicworld_ghost_left(false);
        cl_set_logicworld_ghost_right(false);
        cl_set_logicworld_ghost_up(false);
        cl_set_logicworld_ghost_down(false);

        cl_set_logicworld_ghost_up(true);
        //SetPhyPlayerLinearVel(nullptr, &phySpeedPos, nullptr);
        break;
    case SDLK_DOWN:
        cl_set_logicworld_ghost_left(false);
        cl_set_logicworld_ghost_right(false);
        cl_set_logicworld_ghost_up(false);
        cl_set_logicworld_ghost_down(false);

        cl_set_logicworld_ghost_down(true);
        //SetPhyPlayerLinearVel(nullptr, &phySpeedNeg, nullptr);
        break;
    case SDLK_SPACE:
        cl_set_logicworld_ghost_jump(true);
        //SetPhyPlayerLinearVel(nullptr, nullptr, &phyJumpSpeed);
        break;
    }
}

static void StartChatInput()
{
    if (gChatMode == FALSE)
    {
        SDL_StartTextInput();

        gImeText = gChatLineText;
        gCompositingText = gCompositingChatLineText;

        gChatMode = TRUE;

        InterlockedExchange(&gRedrawCurrentIme, 1);
    }
}

static void StopChatInput()
{
    if (gChatMode == TRUE)
    {
        SDL_StopTextInput();

        gChatMode = FALSE;

        InterlockedExchange(&gRedrawCurrentIme, 1);
    }
}

static void AddChatLine(const std::wstring& line)
{
    std::vector<std::wstring> tok;
    boost::split(tok, line, boost::is_any_of(L"\n"));
    for (const std::wstring& s : tok)
    {
        gChatLog.push_back(s);
    }

    InterlockedExchange(&gRedrawCurrentIme, 1);
}

static int AlertCallback(const char* message)
{
    wchar_t textw[4096];
    MultiByteToWideChar(CP_UTF8, 0, message, strlen(message) + 1, textw, _countof(textw));
    ShowSingleLineScreenText(textw, 14, 2500);

    return 0;
}

static int VoiceCallback(const char* buffer, size_t buflen)
{
    PlayMp3((void*)buffer, buflen);

    return 0;
}

static int UpdateHpCallback()
{
    rs_set_fullscreen_backbuffer_scale(0.95f);

    return 0;
}

static void ChatOutputHandler(int id, const char* line)
{
    const char* n = cl_entity_name(id);

    std::wstring whow = n ? wstring_from_utf8(cl_entity_name(id)) : L"<?>";
    std::wstring linew = line ? wstring_from_utf8(line) : L"";

    AddChatLine(whow + L": " + linew);
}

static void EventPushedEventHandler()
{
    SDL_Event logicEvent;
    logicEvent.type = SDL_USEREVENT;
    SDL_PushEvent(&logicEvent);
}

int ProcessProgramOptions(int ac, char *av[])
{
    namespace po = boost::program_options;
    using namespace std;

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help", "produce help message")
        ("vsync", "V-sync")
        ("compression", po::value<int>(), "set compression level")
        ("username", po::value<std::string>(), "user name")
        ("server", po::value<std::string>(), "server address")
        ("ogl43", "use opengl 4.3")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(ac, av, desc), vm);
    po::notify(vm);    

    if (vm.count("help"))
    {
        cout << desc << "\n";
        return 1;
    }

    if (vm.count("compression"))
    {
        cout << "Compression level was set to " << vm["compression"].as<int>() << ".\n";
    }
    else
    {
        cout << "Compression level was not set.\n";
    }

    bool userNameProvided = false;
    bool serverProvided = false;

    if (vm.count("username"))
    {
        std::string username = vm["username"].as<std::string>();

        cout << "당신의 이름은 " << username.c_str() << "입니다.\n";

        wchar_t* converted = SDL_create_wide_str(username.c_str());

        StringCbCopy(gUserNameText, sizeof(gUserNameText), converted);

        SDL_free(converted);

        userNameProvided = (wcslen(gUserNameText) > 0);
    }

    if (vm.count("server"))
    {
        std::string server = vm["server"].as<std::string>();

        cout << "당신이 들어가려는 세계의 주소는 " << server.c_str() << "입니다.\n";

        wchar_t* converted = SDL_create_wide_str(server.c_str());

        StringCbCopy(gWorldNameText, sizeof(gWorldNameText), converted);

        SDL_free(converted);

        serverProvided = (wcslen(gWorldNameText) > 0);
    }

    gFastLogin = (userNameProvided && serverProvided);

    // OpenGL 4.3 모드가 되면 디버그 환경이 좀 나아지기 때문에 (디버그 메시지 콜백 활성화됨)
    // 쓰는 것이 좋다. 그러나 실행만을 위한 것이라면 OpenGL 3.0으로 충분하므로
    // 이건 옵션으로 둔다.
    gOpenGl43 = vm.count("ogl43") ? true : false;

    return 0;
}

void Login(const char* username, const char* worldname)
{
    FILE* f = fopen("config.txt", "w");
    fwprintf(f, L"%s\n%s\n", gUserNameText, gWorldNameText);
    fclose(f);

    cl_connect_to_server(worldname);
    cl_enter_world(username);
    cl_start_network_thread();
}

void Login(const wchar_t* gUserNameText, const wchar_t* gWorldNameText)
{
    InterlockedExchange(&gRedrawCurrentIme, 1);

    char username[4096], worldname[4096];
    WideCharToMultiByte(CP_UTF8, 0, gUserNameText, -1, username, sizeof(username), NULL, NULL);
    WideCharToMultiByte(CP_UTF8, 0, gWorldNameText, -1, worldname, sizeof(worldname), NULL, NULL);

    Login(username, worldname);
}

void ProcessPhyMoveCommands()
{
    for (auto v : gKeyVector)
    {
        SendPhyCommandMoveByKey(v);
    }
}

static int GetKeyModifierState()
{
    int key_modifier_state = 0;

    // Query the state of all modifier keys
    if (GetKeyState(VK_CAPITAL) & 1)
    {
        key_modifier_state |= Rocket::Core::Input::KM_CAPSLOCK;
    }

    if (HIWORD(GetKeyState(VK_SHIFT)) & 1)
    {
        key_modifier_state |= Rocket::Core::Input::KM_SHIFT;
    }

    if (GetKeyState(VK_NUMLOCK) & 1)
    {
        key_modifier_state |= Rocket::Core::Input::KM_NUMLOCK;
    }

    if (HIWORD(GetKeyState(VK_CONTROL)) & 1)
    {
        key_modifier_state |= Rocket::Core::Input::KM_CTRL;
    }

    if (HIWORD(GetKeyState(VK_MENU)) & 1)
    {
        key_modifier_state |= Rocket::Core::Input::KM_ALT;
    }

    return key_modifier_state;
}

int main(int argc, char *argv[])
{
    printf("clientsdl (" __DATE__ " " __TIME__ ")\n");

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        fprintf(stderr, 
            "\nUnable to initialize SDL:  %s\n", 
            SDL_GetError()
            );
    }

    int i;
    SDL_Event event;
    mmocl_event cl_event;
    Uint32 then, now, frames;
    int jumpnow = 0;

    RenderingData renderingThreadData;

    FILE* f = fopen("config.txt", "r");
    if (f)
    {
        fwscanf(f, L"%s\n%s\n", gUserNameText, gWorldNameText);
        fclose(f);
    }
    else
    {
        //StringCbCopy(gUserNameText, sizeof(gUserNameText), L"ben");
        StringCbCopy(gWorldNameText, sizeof(gWorldNameText), L"localhost");
    }

    ProcessProgramOptions(argc, argv);

    InterlockedExchange(&gRedrawCurrentIme, 1);

    printf("PID: %d\n", GetCurrentProcessId());

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

            /*
            fprintf(stderr, "Usage: %s %s [--fsaa] [--accel n]\n", argv[0],
            CommonUsage(state));
            quit(1);
            */

            consumed = 1;
        }

        i += consumed;
    }

    /* Set OpenGL parameters */
    state->window_flags |= SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
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

    SDL_Surface *surface;     // Declare an SDL_Surface to be filled in with pixel data from an image file
    surface = SDL_CreateRGBSurfaceFrom((void*)gimp_image.pixel_data, gimp_image.width, gimp_image.height, gimp_image.bytes_per_pixel * 8, gimp_image.width * 4, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

    SDL_SetWindowIcon(state->windows[0], surface);

    char window_title[128];
    StringCbPrintfA(window_title, sizeof(window_title), "MMO (%s)", SDL_GetWindowTitle(state->windows[0]));
    SDL_SetWindowTitle(state->windows[0], window_title);

    SDL_FreeSurface(surface);

    /* Main render loop */
    frames = 0;
    then = SDL_GetTicks();
    done = 0;

    SetUiMode(gUiMode);

    glhInit();

    gChatLog.resize(CHAT_LOG_LINES);

    InitializeFontSystem();

    mmocl_context ctx;
    cl_initialize_params ctxParams;
    memset(&ctxParams, 0, sizeof(cl_initialize_params));
    ctxParams.chat_output_handler = ChatOutputHandler;
    ctxParams.event_pushed_event_handler = EventPushedEventHandler;
    cl_initialize_context(&ctx, &ctxParams);
    cl_list_sprites();

    cl_py_set_alert_callback(AlertCallback);
    cl_py_set_voice_callback(VoiceCallback);
    cl_py_set_update_hp_callback(UpdateHpCallback);

    Uint32 eventStartNo = SDL_RegisterEvents(1);
    assert(eventStartNo == SDL_USEREVENT);

    bool enable_rendering_thread = false;

    renderingThreadData.state = state;
    SDL_Thread* RenderingThreadObject = nullptr;

    std::shared_ptr<RenderingModule> rtm;
    if (enable_rendering_thread)
    {
        //SDL_CreateThread(RenderingThread, "RenderingThread", &renderingThreadData);
    }
    else
    {
        rtm.reset(new RenderingModule(&renderingThreadData));
    }

    
    memset(player.rot, 0, sizeof(player.rot));
    player.rot[0] = 1.0f;
    player.rot[5] = 1.0f;
    player.rot[10] = 1.0f;
    //SDL_CreateThread(PhysicsThread, "PhysicsThread", NULL);
    SDL_Thread* AudioThreadObject = SDL_CreateThread(AudioThread, "AudioThread", NULL);

    if (gFastLogin)
    {
        ToggleUiMode();
        Login(gUserNameText, gWorldNameText);
    }

    double dt = 0;

    while (!done)
    {
        const Uint64 perf_count_then = SDL_GetPerformanceCounter();
        
        ++frames;

        jumpnow = 0;

        if (gContext)
        {
            gContext->Update();
        }

        if (rtm)
        {
            rtm->Tick(static_cast<float>(dt));
        }

        //ProcessPhyMoveCommands();

        while (!done && /*SDL_WaitEvent(&event)*/ SDL_PollEvent(&event)) {

            //ProcessPhyMoveCommands();

            while (cl_poll_event(&cl_event) == CEC_OKAY)
            {
                switch (cl_event.type)
                {
                case MET_SERVERS:
                    {
                        ServerListDataSource::ClearServer();
                    }
                    break;
                case MET_SERVER:
                    {
                        ServerListDataSource::AddServer(
                            cl_event.server.host,
                            cl_event.server.port,
                            cl_event.server.name
                            );
                    }
                    break;
                case MET_CONNECT_FAIL:
                    {
                        while (auto pDoc = gContext->GetDocument("modalwait"))
                        {
                            pDoc->Close();

                            /*auto pDoc = gContext->GetDocument(i);
                            const auto& id = pDoc->GetId();
                            cl_debug_output("DOC ID: %s", id.CString());*/
                        }
                        while (auto pDoc = gContext->GetDocument("chat"))
                        {
                            pDoc->Close();

                            /*auto pDoc = gContext->GetDocument(i);
                            const auto& id = pDoc->GetId();
                            cl_debug_output("DOC ID: %s", id.CString());*/
                        }
                        bool loadSuccess = EventManager::LoadWindow("background");
                        assert(loadSuccess);
                    }
                    break;
                case MET_ENTER_WORLD:
                    while (auto pDoc = gContext->GetDocument("background"))
                    {
                        pDoc->Close();
                    }

                    //SetUiMode(FALSE);
                    const char* world_name = cl_world_name(cl_event.enter_world.world_id);

                    wchar_t* converted = SDL_create_wide_str(world_name);

                    wchar_t textw[4096] = {0};
                    StringCbCat(textw, sizeof(textw), converted);
                    StringCbCat(textw, sizeof(textw), L"에 온 것을 환영한다!");

                    SDL_free(converted);

                    ShowSingleLineScreenText(textw, 14, 2500);
                    StartChatInput();

                    if (gContext)
                    {
                        if (auto pDoc = gContext->GetDocument("modalwait"))
                        {
                            pDoc->Close();
                        }

                        EventManager::LoadWindow("chat");
                    }
                    break;
                }
            }

            //switch (event.type) {
            //case SDL_WINDOWEVENT:
            //    switch (event.window.event) {
            //    case SDL_WINDOWEVENT_CLOSE:
            //        {
            //            quit(0);
            //        }
            //        break; 
            //    }
            //    break;
            //}

            CommonEvent(state, &event, &done);
            
            switch (event.type)
            {
            case SDL_TEXTINPUT:
                /* Add new text onto the end of our text */
                {
                    wchar_t textw[4096];
                    MultiByteToWideChar(CP_UTF8, 0, event.text.text, strlen(event.text.text) + 1, textw, _countof(textw));
                    // gImeText가 오버플로우 되는 버그 있음
                    /*
                    wcscat(gImeText, textw);
                    */
                    
                    // 텍스트 입력 (UTF-8이 아니라 UCS-2로 보내면 된다.)
                    //gContext->ProcessTextInput(event.text.text[0]);
                    gContext->ProcessTextInput(textw[0]);

                    InterlockedExchange(&gRedrawCurrentIme, 1);
                }
                break;
            case SDL_TEXTEDITING:
                {
                    /*wchar_t textw[4096];
                    MultiByteToWideChar(CP_UTF8, 0, event.edit.text, strlen(event.edit.text) + 1, textw, _countof(textw));
                    cursor = event.edit.start;
                    selection_len = event.edit.length;
                    wcscpy(gCompositingText, textw);

                    InterlockedExchange(&gRedrawCurrentIme, 1);*/
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (gContext)
                {
                    gContext->ProcessMouseMove(event.motion.x, event.motion.y, GetKeyModifierState());
                    gContext->ProcessMouseButtonDown(0, GetKeyModifierState());
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (gContext)
                {
                    gContext->ProcessMouseMove(event.motion.x, event.motion.y, GetKeyModifierState());
                    gContext->ProcessMouseButtonUp(0, GetKeyModifierState());
                }
                break;

            case SDL_KEYDOWN:

                if (gContext)
                {
                    // 텍스트 입력(IME)과는 관련 없다. 텍스트 입력으로서가 아닌 키입력만을 처리함. -_-;
                    if (gContext->ProcessKeyDown(TranslateSDLKey(event.key.keysym.sym), GetKeyModifierState()))
                    {
                        //break;
                    }
                }

                switch (event.key.keysym.sym) {
                case SDLK_LCTRL:
                    gFire =  1;
                    break;

                case SDLK_LEFT:
                case SDLK_RIGHT:
                case SDLK_UP:
                case SDLK_DOWN:
                    SendPhyCommandMoveByKey(event.key.keysym.sym);

                    if (event.key.repeat == 0)
                    {
                        SendEntitiyCommandMoveByKey(event.key.keysym.sym);

                        if (std::find(gKeyVector.begin(), gKeyVector.end(), event.key.keysym.sym) == gKeyVector.end())
                        {
                            gKeyVector.push_back(event.key.keysym.sym);
                        }
                    }
                    break;
                case SDLK_SPACE:
                    if (event.key.repeat == 0)
                    {
                        SendPhyCommandMoveByKey(event.key.keysym.sym);

                        jumpnow = 1;

                        kcv.jump = 1;
                    }
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

                                InterlockedExchange(&gRedrawCurrentIme, 1);
                            }
                            else if (ch > 0)
                            {
                                gImeText[ch-1] = L'\0';

                                InterlockedExchange(&gRedrawCurrentIme, 1);
                            }
                        }
                    }
                    break;

                case SDLK_F4:
                    //cl_spawn_entity();
                    break;

                case SDLK_F9:
                    cl_player_chat("/walk");
                    break;

                }
                break;
            case SDL_KEYUP:
                if (gContext)
                {
                    // 텍스트 입력(IME)과는 관련 없다. 텍스트 입력으로서가 아닌 키입력만을 처리함. -_-;
                    if (gContext->ProcessKeyUp(TranslateSDLKey(event.key.keysym.sym), GetKeyModifierState()))
                    {
                        //break;
                    }
                }

                switch (event.key.keysym.sym) {
                case SDLK_KP_PLUS:
                    sunTheta = clampf(sunTheta + 0.1f, 0, (float)M_PI);
                    break;
                case SDLK_KP_MINUS:
                    sunTheta = clampf(sunTheta - 0.1f, 0, (float)M_PI);
                    break;

                case SDLK_LCTRL:
                    gFire = 0;
                    break;

                case SDLK_LEFT:
                case SDLK_RIGHT:
                case SDLK_UP:
                case SDLK_DOWN:
                    gKeyVector.erase(std::remove(gKeyVector.begin(), gKeyVector.end(), event.key.keysym.sym), gKeyVector.end());
                    if (gKeyVector.empty())
                    {
                        cl_entity_command(CAC_ENTITY_MOVE_STOP, cl_player_id());
                        static const float ZERO = 0.0f;

                        //SetPhyPlayerLinearVel(&ZERO, &ZERO, &ZERO);
                        //SetPhyPlayerLinearVelZero();

                        cl_set_logicworld_ghost_left(false);
                        cl_set_logicworld_ghost_right(false);
                        cl_set_logicworld_ghost_up(false);
                        cl_set_logicworld_ghost_down(false);
                        //cl_set_logicworld_ghost_jump(false);
                    }
                    else
                    {
                        SDL_Keycode kc = *gKeyVector.rbegin();

                        SendEntitiyCommandMoveByKey(kc);

                        SendPhyCommandMoveByKey(kc);
                    }
                    break;

                case SDLK_SPACE:
                    kcv.jump = 0;
                    cl_set_logicworld_ghost_jump(false);
                    break;

                case SDLK_F1:
                    cl_player_set_move_speed(30.0f);
                    break;

                case SDLK_F2:
                    ToggleUiMode();
                    break;

                case SDLK_F3:
                    {
                        static int c = 0;
                        wchar_t textw[1024];
                        StringCbPrintf(textw, sizeof(textw), L"실행완료! [%d]", ++c);
                        ShowSingleLineScreenText(textw, 14, 2500);
                    }
                    break;

                case SDLK_F4:
                    cl_spawn_cylinder();
                    break;

                case SDLK_F5:
                    cl_interact_heal();
                    break;

                case SDLK_F6:
                    cl_interact_attack();
                    break;

                case SDLK_F7:
                    cl_request_server_list();
                    break;

                case SDLK_F8:
                    {
                        double x, y, z;
                        cl_player_pos(&x, &y);
                        cl_player_pos_z(&z);
                        SetPhyPlayerPosition((float)x, (float)y, (float)z + 30.0f);
                    }
                    break;

                case SDLK_TAB:
                    /*if (gUiMode)
                    {
                        if (gImeText == NULL || gImeText == gWorldNameText)
                        {
                            SDL_StopTextInput();
                            SDL_StartTextInput();

                            gImeText = gUserNameText;
                            gCompositingText = gCompositingUserNameText;
                        }
                        else
                        {
                            SDL_StopTextInput();
                            SDL_StartTextInput();

                            gImeText = gWorldNameText;
                            gCompositingText = gCompositingWorldNameText;
                        }
                    }*/
                    break;
                case SDLK_RETURN:
                    {
                        /*
                        if (event.key.repeat == 0)
                        {
                            if (gUiMode && wcslen(gUserNameText) > 0 && wcslen(gWorldNameText) > 0)
                            {
                                Login(gUserNameText, gWorldNameText);
                            }
                            else if (gChatMode && wcslen(gChatLineText) > 0)
                            {
                                if (cl_is_player_valid())
                                {
                                    cl_player_chat(to_utf8(gChatLineText).c_str());
                                }

                                gChatLineText[0] = L'\0';

                                InterlockedExchange(&gRedrawCurrentIme, 1);
                            }
                        }
                        */
                    }

                    break;

                }
                break;

            case SDL_MOUSEWHEEL:
                camdist -= event.wheel.y / 15.0f;
                break;

            case SDL_MOUSEMOTION:
                if (!gUiMode && gTrackballView)
                {
                    campitch = clampf(campitch + event.motion.yrel / 250.0f, (float)M_PI/4 - (float)M_PI/32, (float)M_PI/4 + (float)M_PI/32);

                    //campitch = clampf(campitch + event.motion.yrel / 250.0f, 0, (float)M_PI/2);
                    //camyaw = clampf(camyaw - event.motion.xrel / 250.0f, (float)-M_PI, (float)M_PI);
                }

                if (gContext)
                {
                    //gContext->Rocket::Core::Context::ProcessMouseMove(event.motion.x, event.motion.y, GetKeyModifierState());
                    gContext->ProcessMouseMove(event.motion.x, event.motion.y, GetKeyModifierState());
                }

                break;
            }
        }

        const Uint64 perf_freq = SDL_GetPerformanceFrequency();

        Uint64 perf_count_diff = SDL_GetPerformanceCounter() - perf_count_then;
        static const double target_dt = 1.0 / 30;
        dt = static_cast<double>(perf_count_diff) / perf_freq;
        const double sleep_time = target_dt - dt;
        if (sleep_time > 0)
        {
            const Uint64 targetSleepTick = static_cast<Uint64>(sleep_time * perf_freq);
            Uint64 cur = SDL_GetPerformanceCounter();
            while (true)
            {
                if (SDL_GetPerformanceCounter() - cur > targetSleepTick)
                {
                    break;
                }
            }
        }

        perf_count_diff = SDL_GetPerformanceCounter() - perf_count_then;
        dt = static_cast<double>(perf_count_diff) / perf_freq;

        UpdateDebugPrint(static_cast<float>(dt));
    }

    extern int gDoRenderingLoop;
    gDoRenderingLoop = 0;

    int renderingLoopStatus;
    if (RenderingThreadObject)
    {
        SDL_WaitThread(RenderingThreadObject, &renderingLoopStatus);
    }

    /* Print out some timing information */
    now = SDL_GetTicks();
    if (now > then) {
        printf("%2.2f frames per second\n",
            ((double) frames * 1000) / (now - then));
    }

    SDL_DestroyWindow(state->windows[0]);

    return 0;
}

static void
    UnEscapeQuotes(char *arg)
{
    char *last = NULL;

    while (*arg) {
        if (*arg == '"' && (last != NULL && *last == '\\')) {
            char *c_curr = arg;
            char *c_last = last;

            while (*c_curr) {
                *c_last = *c_curr;
                c_last = c_curr;
                c_curr++;
            }
            *c_last = '\0';
        }
        last = arg;
        arg++;
    }
}

/* Parse a command line buffer into arguments */
static int
    ParseCommandLine(char *cmdline, char **argv)
{
    char *bufp;
    char *lastp = NULL;
    int argc, last_argc;

    argc = last_argc = 0;
    for (bufp = cmdline; *bufp;) {
        /* Skip leading whitespace */
        while (isspace(*bufp)) {
            ++bufp;
        }
        /* Skip over argument */
        if (*bufp == '"') {
            ++bufp;
            if (*bufp) {
                if (argv) {
                    argv[argc] = bufp;
                }
                ++argc;
            }
            /* Skip over word */
            lastp = bufp;
            while (*bufp && (*bufp != '"' || *lastp == '\\')) {
                lastp = bufp;
                ++bufp;
            }
        } else {
            if (*bufp) {
                if (argv) {
                    argv[argc] = bufp;
                }
                ++argc;
            }
            /* Skip over word */
            while (*bufp && !isspace(*bufp)) {
                ++bufp;
            }
        }
        if (*bufp) {
            if (argv) {
                *bufp = '\0';
            }
            ++bufp;
        }

        /* Strip out \ from \" sequences */
        if (argv && last_argc != argc) {
            UnEscapeQuotes(argv[last_argc]);
        }
        last_argc = argc;
    }
    if (argv) {
        argv[argc] = NULL;
    }
    return (argc);
}

/* Show an error message */
static void
    ShowError(const char *title, const char *message)
{
    /* If USE_MESSAGEBOX is defined, you need to link with user32.lib */
#ifdef USE_MESSAGEBOX
    MessageBox(NULL, message, title, MB_ICONEXCLAMATION | MB_OK);
#else
    fprintf(stderr, "%s: %s\n", title, message);
#endif
}

/* Pop up an out of memory message, returns to Windows */
static BOOL
    OutOfMemory(void)
{
    ShowError("Fatal Error", "Out of memory - aborting");
    return FALSE;
}

class outbuf : public std::streambuf {
public:
    outbuf() {
        setp(0, 0);
    }

    virtual int_type overflow(int_type c = traits_type::eof()) {
        return _fputtc(c, stdout) == EOF ? traits_type::eof() : c;
    }
};

class woutbuf : public std::wstreambuf {
public:
    woutbuf() {
        setp(0, 0);
    }

    virtual int_type overflow(int_type c = traits_type::eof()) {
        return _fputtc(c, stdout) == EOF ? traits_type::eof() : c;
    }
};

int WINAPI
    WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
    // create the console
    if(AllocConsole()) {
        FILE* o = _tfreopen(_T("CONOUT$"), _T("w"), stdout);
        SetConsoleTitle(_T("Debug Console"));
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);  

#ifdef _UNICODE
        //_setmode(_fileno(stdout), _O_WTEXT);
#endif
    }

    _putts(TEXT("하이룽"));

    // set std::cout to use my custom streambuf
    outbuf ob;
    woutbuf wob;
    std::streambuf *sb = std::cout.rdbuf(&ob);
    std::wstreambuf *wsb = std::wcout.rdbuf(&wob);

    //std::cout << "std::cout 테스트\n";
    std::cout << L"std::wcout 테스트\n";

    char **argv;
    int argc;
    char *cmdline;

    /* Grab the command line */
    TCHAR *text = GetCommandLine();
#if UNICODE
    cmdline = SDL_iconv_string("UTF-8", "UCS-2-INTERNAL", (char *)(text), (SDL_wcslen(text)+1)*sizeof(WCHAR));
#else
    cmdline = SDL_strdup(text);
#endif
    if (cmdline == NULL) {
        return OutOfMemory();
    }

    /* Parse it into argv and argc */
    argc = ParseCommandLine(cmdline, NULL);
    argv = SDL_stack_alloc(char *, argc + 1);
    if (argv == NULL) {
        return OutOfMemory();
    }
    ParseCommandLine(cmdline, argv);



    /* Run the main program */
    main(argc, argv);

    SDL_free(cmdline);

    // make sure to restore the original so we don't get a crash on close!
    std::cout.rdbuf(sb);
    std::wcout.rdbuf(wsb);

    /* Hush little compiler, don't you cry... */
    return 0;
}

/* vi: set ts=4 sw=4 expandtab: */
