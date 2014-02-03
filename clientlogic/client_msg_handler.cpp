#include "stdafx.h"
#include "msg_writer.h"
#include "msg_handler_map.h"
#include "game_msg.h"
#include "system_msg.h"
#include "resource_msg.h"
#include "pylon_msg.h"
#include "entity.h"
#include "entity_manager.h"
#include "component_sid.h"
#include "component_name.h"
#include "component_pos0.h"
#include "component_pos1.h"
#include "component_anim.h"
#include "component_sync.h"
#include "component_entity_template.h"
#include "component_hp.h"
#include "clientlogic.h"
#include "npc_data.h"
#include "world_data.h"
#include "ClContext.h"
#include "logger.h"
#include "event.h"

using namespace client;

static int g_player_sid;

void ClSendGameEvent(int event_type);

inline client::entity_manager* ClCtxEm();

void ClPushEvent(const mmocl_event& e);

spawn_callback_type spawn_callback = nullptr;
CLIENTLOGIC_API int cl_py_set_spawn_callback(spawn_callback_type fcb)
{ spawn_callback = fcb; return 0; }

world_info_callback_type world_info_callback = nullptr;
CLIENTLOGIC_API int cl_py_set_world_info_callback(world_info_callback_type fcb)
{ world_info_callback = fcb; return 0; }

alert_callback_type alert_callback = nullptr;
CLIENTLOGIC_API int cl_py_set_alert_callback(alert_callback_type fcb)
{ alert_callback = fcb; return 0; }

voice_callback_type voice_callback = nullptr;
CLIENTLOGIC_API int cl_py_set_voice_callback(voice_callback_type fcb)
{ voice_callback = fcb; return 0; }

update_hp_callback_type update_hp_callback = nullptr;
CLIENTLOGIC_API int cl_py_set_update_hp_callback(update_hp_callback_type fcb)
{ update_hp_callback = fcb; return 0; }

MSG_HANDLER(world_info)
{
    data::world_data d;
    const std::string& world_name = d[msg.world_id]->world_name->name;

    ClDebugOutput("===World Info===\n");
    ClDebugOutput("  World sid = %d (%s)\n", msg.world_id, world_name.c_str());
    ClDebugOutput("  User  sid = %d\n", msg.id);

    g_player_sid = msg.id;

    if (world_info_callback)
        world_info_callback(world_name.c_str());

    mmocl_event e;
    e.enter_world.type = MET_ENTER_WORLD;
    e.enter_world.world_id = msg.world_id;
    ClPushEvent(e);
}

MSG_HANDLER(spawn)
{
    ClDebugOutput("SPAWN: sid=%d, name=%s, pos_x=%lf, pos_y=%lf, speed=%lf, dir=%lf\n",
        msg.id,
        msg.name.c_str(),
        msg.update_position.x,
        msg.update_position.y,
        msg.update_position.speed,
        msg.update_position.dir);

    while (ClCtxEm() == nullptr) ;

    client::entity e = ClCtxEm()->new_entity(msg.id);

    get_as<component::sid>(e).v = msg.id;
    get_as<component::name>(e).n = msg.name;
    get_as<component::name>(e).nameplate_visible = true;
    get_as<component::anim>(e).interp = false; // 스폰 시에는 순간 이동 해야함

    // 플레이어 스폰
    if (g_player_sid == msg.id)
    {
        ClCtxEm()->set_player(e);

        ClCtxEm()->plug<component::sync>(e);

        ClSendGameEvent(CGE_PLAYER_SPAWNED);
    }

    if (spawn_callback)
        spawn_callback(msg.id, msg.update_position.x, msg.update_position.y);
}

template <typename T, typename U>
double Distance2DSquared(const T& t, const U& u)
{
    const double dx = (t.x - u.x);
    const double dy = (t.y - u.y);
    return dx*dx + dy*dy;
}

MSG_HANDLER(update_position)
{
    ClDebugOutput("UPDATE_POSITION: sid=%d, x=%lf, y=%lf, dir=%lf, speed=%lf, instance_move=%d\n", msg.id, msg.x, msg.y, msg.dir, msg.speed, msg.instance_move);

    while (ClCtxEm() == nullptr) ;

    client::entity e(msg.id);

    if(!e.is_valid())
        return;

    // 현재 위치
    auto& o_pos0 = get_as<component::pos0>(e);

    // 이동 목표 지점
    auto& o_pos1 = get_as<component::pos1>(e);
    o_pos1.x = msg.x;
    o_pos1.y = msg.y;

    // 바라보는 방향을 설정
    auto& o_anim = get_as<component::anim>(e);
    o_anim.cur_dir = move_from_direction(msg.dir);
    o_anim.move_speed = msg.speed;

    if (!o_anim.interp || msg.instance_move) {

        // 현재 위치 직접 변경 - 순간이동

        o_pos0.x = msg.x;
        o_pos0.y = msg.y;

        o_anim.interp = true; // 순간이동 해제

    } else {

        // 목표 지점 근처에 왔다면 애니메이션 없음
        if (Distance2DSquared(o_pos0, o_pos1) < 5)
        {
            o_anim.cur_move = EMove::STOP;
        }
        else
        {
            o_anim.cur_move = o_anim.cur_dir;
        }
    }
}

MSG_HANDLER(despawn)
{
    auto& entity_array = ClCtxEm()->get_entities();

    for (auto it = entity_array.begin(), it_end = entity_array.end(); it != it_end; ++it)
    {
        if (it->second.get_id() == msg.id)
        {
            ClChatOutput(0, (get_as<component::name>(it->second).n + " despawn").c_str());

            it->second.invalidate();
        }
    }
}

MSG_HANDLER(chat)
{
    ClChatOutput(msg.id, msg.message.c_str());
}

MSG_HANDLER(session_error)
{
    // 995: 스레드 종료 또는 응용 프로그램 요청 때문에 I/O 작업이 취소되었습니다. 
    // 1236: 네트워크 연결이 로컬 시스템에 의해 취소되었습니다.
    if (msg.error_code == 1236 || msg.error_code == 995)
        return;

    ClDebugOutput("Gone, server, gone.\n");

    mmocl_event e;
    e.type = MET_CONNECT_FAIL;
    ClPushEvent(e);
}

MSG_HANDLER(character_resource)
{
    auto& entity_array = ClCtxEm()->get_entities();

    for (auto it = entity_array.begin(), it_end = entity_array.end(); it != it_end; ++it)
    {
        if (it->second.get_id() == msg.id)
        {
            ClCtxEm()->get_as<component::entity_template>(it->second).resource_name = msg.resource_id;
            return;
        }
    }
}

MSG_HANDLER(voice)
{
    if (voice_callback)
        voice_callback(msg.mp3.get_buffer(), msg.mp3.size());
}

MSG_HANDLER(update_hp)
{
    ClDebugOutput("UPDATE_HP: sid=%d, %d/%d\n", msg.id, msg.hp, msg.max_hp);

    auto& entity_array = ClCtxEm()->get_entities();

    for (auto it = entity_array.begin(), it_end = entity_array.end(); it != it_end; ++it)
    {
        if (it->second.get_id() == msg.id)
        {
            ClCtxEm()->get_as<component::hp>(it->second).cur_hp = msg.hp;
            ClCtxEm()->get_as<component::hp>(it->second).max_hp = msg.max_hp;

            if (update_hp_callback)
                update_hp_callback();
        }
    }
}

MSG_HANDLER(servers)
{
    // server_msg를 위해 더미로 넣은 핸들러
    ClDebugOutput("servers_msg received...\n");

    extern tbb::concurrent_vector<msg::server_msg> l_server_list;

    l_server_list.clear();

    mmocl_event e;
    e.type = MET_SERVERS;
    ClPushEvent(e);
}

MSG_HANDLER(server)
{
    ClDebugOutput("Available server '%s' (%s:%d)\n", msg.name.c_str(), msg.host.c_str(), msg.port);

    extern tbb::concurrent_vector<msg::server_msg> l_server_list;

    l_server_list.push_back(msg);

    mmocl_event e;
    e.type = MET_SERVER;
    e.server.port = msg.port;
    StringCbCopyA(e.server.name, sizeof(e.server.name), msg.name.c_str());
    StringCbCopyA(e.server.host, sizeof(e.server.host), msg.host.c_str());
    ClPushEvent(e);
}

MSG_HANDLER(alert)
{
    if (alert_callback)
        alert_callback(msg.message.c_str());
}
