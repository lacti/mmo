#include "stdafx.h"
#include "clientlogic.h"
#include "clientlogic_private.h"
#include "entity_manager.h"
#include "component_pos0.h"
#include "component_pos1.h"
#include "component_anim.h"
#include "component_name.h"
#include "component_sid.h"
#include "component_sync.h"
#include "component_entity_template.h"
#include "component_seqanim.h"
#include "component_hp.h"
#include "animation.inl"
#include <msggen/msg_writer.h>
#include <msggen/msg_session.h>
#include "system_msg.h"
#include "pylon_msg.h"
#include "game_msg.h"
#include "sprite_data.h"
#include "static_object_data.h"
#include "world_name_data.h"
#include "interact_template_data.h"
#include "entity_manager_ref.h"
#include "ClContext.h"
#include <logger.h>
#include <utf8.h>
#include "../ProcMap/ProcMap.h"
#include "tilemap.h"
#include <strsafe.h>
#include "LogicWorld.h"
#include <assert.h>
#include "component_static_object_template.h"

std::vector<ClContext*> l_ctx_array;
mmocl_context l_bound_ctx;
tbb::concurrent_vector<msg::server_msg> l_server_list;

void ClDebugOutput(const char* format, ...)
{
    static const size_t max_length = 4096;
    char buffer[max_length];
    va_list vaList;
    va_start(vaList, format);
    _vsnprintf(buffer, max_length, format, vaList);
    va_end(vaList);

    if (ClCtx()->debug_output_handler)
        ClCtx()->debug_output_handler(buffer);

    if (buffer[strlen(buffer)-1] == '\n')
        buffer[strlen(buffer)-1] = '\0';

    LAPP_ << string949_from_utf8(std::string(buffer));
}

void ClChatOutput(int id, const char* line)
{
    ClCtx()->chat_output_handler(id, line);
}

void ClSendGameEvent(int event_type)
{
    if (ClCtx()->game_event_handler)
        ClCtx()->game_event_handler(event_type);
}

int cl_test_func(int v)
{
    return v + 1985;
}

int cl_py_initialize(const char* server, const char* user)
{
    mmocl_context ctx;

    cl_initialize_context(&ctx, nullptr);
    cl_connect_to_server(server);
    cl_enter_world(user);
    cl_start_network_thread();

    return CEC_OKAY;
}

int cl_initialize_context(mmocl_context* ctx, cl_initialize_params* params)
{
    if (ctx == nullptr)
        return CEC_NULL_PARAM;

    auto context = new ClContext;
    l_ctx_array.push_back(context);
    context->em.reset(new client::entity_manager());

    if (params)
    {
        context->debug_output_handler = params->debug_line_output_handler;
        context->chat_output_handler = params->chat_output_handler;
        context->game_event_handler = params->game_event_handler;
        context->event_pushed_event_handler = params->event_pushed_event_handler;
    }

    *ctx = l_ctx_array.size() - 1;

    cl_bind_context(*ctx);

    init_logs_clientlogic();

    InitializeProcMap();

    cltm_init_obstacles();

    context->lw = new LogicWorld();

    return CEC_OKAY;
}

int cl_get_logicworld_ghost_pos(float* pos)
{
    if (ClCtx())
    {
        ClCtx()->lw->GetGhostPos(pos);
    }

    return CEC_OKAY;
}

int cl_set_logicworld_ghost_left(bool b) {  ClCtx()->left = b; return CEC_OKAY; }
int cl_set_logicworld_ghost_right(bool b) {  ClCtx()->right = b; return CEC_OKAY; }
int cl_set_logicworld_ghost_up(bool b) {  ClCtx()->up = b; return CEC_OKAY; }
int cl_set_logicworld_ghost_down(bool b) {  ClCtx()->down = b; return CEC_OKAY; }
int cl_set_logicworld_ghost_jump(bool b) {  ClCtx()->jump = b; return CEC_OKAY; }

int cl_bind_context(mmocl_context ctx)
{
    l_bound_ctx = ctx;

    return CEC_OKAY;
}

int cl_destroy_context()
{
    cl_disconnect_from_server();
    cl_stop_network_thread();

    if (l_ctx_array[l_bound_ctx])
    {
        delete l_ctx_array[l_bound_ctx];
        l_ctx_array[l_bound_ctx] = nullptr;

        return CEC_OKAY;
    }
    else
    {
        return CEC_CONTEXT_NOT_READY;
    }
}

int cl_entity_command(int com, int id)
{
    client::entity e(id);

    if (!e.is_valid())
        return CEC_INVALID_ENTITY;

    double dx = 0, dy = 0;

    auto& anim = C<component::anim>(e);

    double dir = direction_from_move(anim.cur_dir);
    double move_speed_multiplier = 1;
    switch (com)
    {
    case CAC_ENTITY_MOVE_LEFT:
        anim.cur_move = EMove::LEFT;
        dx -= 1000.0;
        dir = M_PI;
        break;

    case CAC_ENTITY_MOVE_RIGHT:
        anim.cur_move = EMove::RIGHT;
        dx += 1000.0;
        dir = 0;
        break;

    case CAC_ENTITY_MOVE_UP:
        anim.cur_move = EMove::UP;
        dy += 1000.0;
        dir = M_PI/2;
        break;

    case CAC_ENTITY_MOVE_DOWN:
        anim.cur_move = EMove::DOWN;
        dy -= 1000.0;
        dir = 3.0/2*M_PI;
        break;

    case CAC_ENTITY_MOVE_STOP:
        anim.cur_move = EMove::STOP;
        move_speed_multiplier = 0;
        break;
    }

    auto& pos0 = ClCtxEm()->get_as<component::pos0>(e);

    double target_x = pos0.x + dx;
    double target_y = pos0.y + dy;

    C<component::pos1>(e).x = target_x;
    C<component::pos1>(e).y = target_y;

    if (H<component::sync>(e))
    {
        msg::move_msg msg(e.get_id(), target_x, target_y, dir, C<component::anim>(e).move_speed * move_speed_multiplier);
        
        ClSessionSend(msg);

        ClDebugOutput("[move_msg][id:%d] %f %f dir %f speed %f", e.get_id(), target_x, target_y, dir, C<component::anim>(e).move_speed * move_speed_multiplier);
    }

    return CEC_OKAY;
}

int cl_frame_move(double dt)
{
    if (ClCtxEm() == nullptr)
        return CEC_CONTEXT_NOT_READY;

    ClCtx()->io_svc.poll();

    auto& list = ClCtxEm()->get_entities();

    int count = 0;
    for (auto&& it : list)
    {
        auto& e = it.second;

        if (ClCtxEm()->has<component::anim>(e) > 0
            && ClCtxEm()->has<component::seqanim>(e) > 0
            && ClCtxEm()->has<component::entity_template>(e) > 0)
        {
            auto& anim = client::get_as<component::anim>(e);
            auto& seqanim = client::get_as<component::seqanim>(e);
            auto& entity_template = client::get_as<component::entity_template>(e);

            seqanim.frame_age += dt;

            if (seqanim.current_frame == -1 || seqanim.frame_age >= seqanim.frame_duration)
            {
                // 다음 프레임으로~

                cl_tex_info cti;
                cl_get_tex_info(entity_template.resource_name, &cti);

                if (cti.umax)
                {
                    ++seqanim.current_frame;

                    if (seqanim.current_frame >= cti.sprite_count)
                    {
                        if (seqanim.loop)
                        {
                            seqanim.current_frame = 0;
                        }
                        else
                        {
                            e.invalidate();
                            continue;
                        }
                    }

                    anim.anim_uindex = seqanim.current_frame % cti.umax;
                    anim.anim_vindex = seqanim.current_frame / cti.umax;

                    anim.anim_vindex = (cti.vmax - 1) - anim.anim_vindex;

                    //seqanim.frame_age -= seqanim.frame_duration;
                    seqanim.frame_age = 0;
                }
            }
        }

        if (ClCtxEm()->has<component::pos0>(e) > 0
            && ClCtxEm()->has<component::pos1>(e) > 0
            && ClCtxEm()->has<component::anim>(e) > 0)
        {
            auto& pos0 = client::get_as<component::pos0>(e);
            auto& pos1 = client::get_as<component::pos1>(e);
            auto& anim = client::get_as<component::anim>(e);

            // 현재 위치
            double x = pos0.x;
            double y = pos0.y;
            double z = pos0.z;

            // 다음(목표) 위치
            const double& xt = pos1.x;
            const double& yt = pos1.y;
            const double& zt = pos1.z;

            // 현재 위치에서 목표 위치를 가리키는 벡터
            const double xd = xt - x;
            const double yd = yt - y;
            const double zd = zt - y;

            const double speed = anim.move_speed;

            const double movement = speed * dt;

            if (fabs(xd) > movement)
            {
                x += ((xd > 0) ? 1 : -1) * movement;
            }
            else
            {
                x = xt;

                if (fabs(yd) > movement)
                {
                    y += ((yd > 0) ? 1 : -1) * movement;
                }
                else
                {
                    y = yt;
                }
            }

            z = zt;

            if (ClCtxEm()->has<component::seqanim>(e) == 0)
            {
                assert(anim.cur_dir != EMove::STOP);

                anim.anim_uindex = move_anim_seq[ anim.cur_dir ][ anim.anim_frame ][ 0 ];
                anim.anim_vindex = move_anim_seq[ anim.cur_dir ][ anim.anim_frame ][ 1 ];

                if (anim.move_frame_duration)
                    anim.anim_frame = static_cast<int>(anim.last_move_duration / (anim.move_frame_duration / anim.move_speed)) % 4;

                if (anim.cur_move != EMove::STOP) {
                    if (anim.cur_dir != anim.cur_move) {
                        anim.cur_dir = anim.cur_move;
                        //anim.last_move_duration = 0;
                    } else {
                        anim.last_move_duration += dt;
                    }
                } else {
                    //anim.anim_frame = static_cast<int>(anim.last_move_duration / anim.move_frame_duration) % 4;

                    if (anim.anim_frame != 0 && anim.anim_frame != 2) {
                        anim.last_move_duration -= dt;
                        anim.anim_frame = static_cast<int>(anim.last_move_duration / (anim.move_frame_duration / anim.move_speed)) % 4;
                    }
                }

                //if (pos0.x == x && pos0.y == y)
                if (pos0.dx == 0 && pos0.dy == 0)
                {
                    //anim.cur_move = EMove::STOP;
                }
            }

            pos0.x = x;
            pos0.y = y;
            pos0.z = z;

            // 지형 따라서~
            /*
            float zPos;
            cl_get_2d_map_query_height(&zPos, (float)pos0.x, (float)pos0.y);
            pos0.z = zPos;
            */

            // 막무가내 오버라이드
            if (client::get_as<component::sid>(e).v == cl_player_id())
            {
                float pos[3];
                cl_get_logicworld_ghost_pos(pos);

                client::get_as<component::pos0>(e).x = pos[0];
                client::get_as<component::pos0>(e).y = pos[1];
                client::get_as<component::pos0>(e).z = pos[2] - 3;

                /*
                client::get_as<component::pos1>(e).x = pos[0];
                client::get_as<component::pos1>(e).y = pos[1];
                client::get_as<component::pos1>(e).z = pos[2] - 3;

                pos0.dx = (pos1.x - pos0.x) / dt;
                pos0.dy = (pos1.y - pos0.y) / dt;
                pos0.dz = (pos1.z - pos0.z) / dt;
                */
            }
        }
    }

    if (ClCtx() && ClCtx()->lw)
    {
        ClCtx()->lw->FrameMove((float)dt);
    }

    return CEC_OKAY;
}

int cl_get_player_anim_index(int* u, int* v)
{
    auto& c = client::get_player_as<component::anim>();
    assert(c.cur_dir != EMove::STOP);

    *u = c.anim_uindex;
    *v = c.anim_vindex;

    return CEC_OKAY;
}

const char* cl_player_name()
{
    return "gb";
}

int cl_start_network_thread()
{
    cl_stop_network_thread();

    ClCtx()->io_svc.reset();

    if (ClCtx()->io_svc_thread)
    {
        assert(0);

        cl_stop_network_thread();
    }

    //ClCtx()->io_svc_thread = new std::thread(boost::bind(&boost::asio::io_service::run, &ClCtx()->io_svc));

    return CEC_OKAY;
}

int cl_stop_network_thread()
{
    if (ClCtx()->io_svc_thread)
    {
        ClCtx()->io_svc.stop();

        if (ClCtx()->io_svc_thread->joinable())
            ClCtx()->io_svc_thread->join();

        delete ClCtx()->io_svc_thread;
        ClCtx()->io_svc_thread = nullptr;
    }

    return CEC_OKAY;
}

int cl_connect_to_server(const char* server, bool for_test /* = false */)
{
    using boost::asio::ip::tcp;
    int ret = CEC_OKAY;
    try
    {
        tcp::resolver resolver(ClCtx()->io_svc);
        tcp::resolver::query query(server, "40000");
        tcp::resolver::iterator iterator = resolver.resolve(query);

        ClCtx()->session.reset(!for_test
            ? new msg_session(ClCtx()->io_svc)
            : new test_msg_session(ClCtx()->io_svc));
        ClCtx()->session->connect(iterator);
    }
    catch (...)
    {
        ret = CEC_NO_NETWORK_SESSION;
        ClDebugOutput("Connection to %s failed!", server);
    }

    return ret;
}

int cl_enter_world(const char* name)
{
    if (ClCtx()->session)
    {
        msg::enter_world_msg msg(name);
        ClSessionSend(msg);

        return CEC_OKAY;
    }

    return CEC_NO_NETWORK_SESSION;
}

int cl_disconnect_from_server()
{
    if (ClCtx()->session)
    {
        ClCtx()->session->close();
        ClCtx()->session.reset();

        ClCtx()->io_svc.stop();
        assert(ClCtx()->io_svc.stopped());
        return CEC_OKAY;
    }

    return CEC_NO_NETWORK_SESSION;
}

int cl_player_chat(const char* line)
{
    if (line == nullptr)
        return CEC_NULL_PARAM;

    if (_stricmp(line, "/walk") == 0)
    {
        double x0, y0;
        cl_player_pos(&x0, &y0);

        int tx0 = (int)(x0/2);
        int ty0 = (int)(y0/2);

        int txt = 5;
        int tyt = 7;

        if (tx0 != txt || ty0 != tyt)
        {
            int tx1, ty1;
            cltm_proceed(tx0, ty0, txt, tyt, &tx1, &ty1);

            ClCtxEm()->get_as<component::pos1>(client::entity(cl_player_id())).x = tx1*2;
            ClCtxEm()->get_as<component::pos1>(client::entity(cl_player_id())).y = ty1*2;
        }

        return CEC_OKAY;
    }

    if (ClCtx()->session)
    {
        const int id = cl_player_id();

        msg::chat_msg msg(id, line);
        ClSessionSend(msg);

        return CEC_OKAY;
    }

    return CEC_NO_NETWORK_SESSION;
}

int cl_player_move()
{
    /*
    msg::move_msg msg(player_id, pos0.x - predicted_movement, pos0.y, M_PI, 0);
    g_session->write(msg);
    */

    return CEC_OKAY;
}

int cl_player_id()
{
    return ClCtxEm() ? client::get_player_as<component::sid>().v : 0;
}

int cl_is_player_valid()
{
    if (ClCtxEm() == nullptr)
        return FALSE;

    return ClCtxEm()->get_player().is_valid() ? TRUE : FALSE;
}

int cl_visible_entity_count()
{
    if (ClCtxEm())
    {
        return ClCtxEm()->get_entities().size();
    }

    return 0;
}

int cl_visible_entities(cl_visible_entity* out_entities, int max_count)
{
    const auto& list = ClCtxEm()->get_entities();

    int count = 0;
    for (auto&& it : list)
    {
        if (count >= max_count)
            break;

        auto& e = it.second;

        if (!e.is_valid())
            continue;

        using namespace component;

        if (H<pos0>(e))
        {
            if (H<seqanim>(e))
            {
                if (C<seqanim>(e).current_frame < 0)
                    continue;
            }

            cl_visible_entity& ve = out_entities[count];

            ve.id = e.get_id();

            ve.x = C<pos0>(e).x;
            ve.y = C<pos0>(e).y;
            ve.z = C<pos0>(e).z;

            if (H<static_object_template>(e))
            {
                ve.resource_name = C<static_object_template>(e).resource_name;
                ve.is_static_object = TRUE;
            }
            else if (H<entity_template>(e))
            {
                ve.resource_name = C<entity_template>(e).resource_name;
                ve.is_static_object = FALSE;
            }
            else
            {
                assert(false);
            }

            if (ve.resource_name == 0)
            {
                ve.resource_name = 1;
            }

            if (H<anim>(e))
            {
                ve.uindex = C<anim>(e).anim_uindex;
                ve.vindex = C<anim>(e).anim_vindex;
            }

            ++count;
        }
    }

    return count;
}

const char* cl_entity_name(int id)
{
    client::entity e(id);
    if (e.is_valid())
    {
        return ClCtxEm()->get_as<component::name>(e).n.c_str();
    }

    return nullptr;
}

int cl_entity_pos(double* x, double* y, int id)
{
#if _DEBUG
    if (x == nullptr || y == nullptr)
        return CEC_NULL_PARAM;
#endif

    client::entity e(id);
    if (e.is_valid())
    {
        *x = ClCtxEm()->get_as<component::pos0>(e).x;
        *y = ClCtxEm()->get_as<component::pos0>(e).y;

        return CEC_OKAY;
    }

    return CEC_INVALID_ENTITY;
}

int cl_entity_pos_z(double* z, int id)
{
#if _DEBUG
    if (z == nullptr)
        return CEC_NULL_PARAM;
#endif

    client::entity e(id);
    if (e.is_valid())
    {
        *z = ClCtxEm()->get_as<component::pos0>(e).z;

        return CEC_OKAY;
    }

    return CEC_INVALID_ENTITY;
}

int cl_player_pos(double* x, double* y)
{
    return cl_entity_pos(x, y, cl_player_id());
}

int cl_player_pos_z(double* z)
{
    return cl_entity_pos_z(z, cl_player_id());
}

int cl_character_sprite_count()
{
    int c = 0;

    data::sprite_data d;
    d.for_each([&] (data::sprite_t* /*item*/) {
        ++c;
    });

    return c;
}

const char* cl_sprite_name(int id)
{
    data::sprite_data d;
    return d[id] ? d[id]->name.c_str() : d[1] ? d[1]->name.c_str() : "";
}

const char* cl_world_name(int id)
{
    data::world_name_data d;
    return d[id] ? d[id]->name.c_str() : d[1] ? d[1]->name.c_str() : "";
}

int cl_list_sprites()
{
    data::sprite_data d;
    d.for_each([=] (data::sprite_t* item) {
        ClDebugOutput("sprite: %s\n", item->name.c_str());
    });

    return CEC_OKAY;
}

int cl_interact()
{
    ClSessionSend(msg::interact_msg());

    return CEC_OKAY;
}

int cl_interact_ex(int interact_id)
{
    ClSessionSend(msg::interact_msg(interact_id));

    return CEC_OKAY;
}

int cl_interact_heal()
{
    ClSessionSend(msg::interact_msg(data::interact_template_t::HEAL));

    return CEC_OKAY;
}

int cl_interact_attack()
{
    ClSessionSend(msg::interact_msg(data::interact_template_t::ATTACK));

    return CEC_OKAY;
}

void cl_debug_output(const char* format, ...)
{
    static const size_t max_length = 4096;
    char buffer[max_length];
    va_list vaList;
    va_start(vaList, format);
    _vsnprintf(buffer, max_length, format, vaList);
    va_end(vaList);

    if (buffer[strlen(buffer)-1] == '\n')
        buffer[strlen(buffer)-1] = '\0';

    LAPP_ << string949_from_utf8(std::string(buffer));
}

int cl_get_tex_info(int resource_name, cl_tex_info* cti)
{
    data::sprite_data d;

    if (cti == nullptr)
    {
        return CEC_NULL_PARAM;
    }

    if (d[resource_name] == nullptr)
    {
        memset(cti, 0, sizeof(cl_tex_info));
        return CEC_INVALID_INDEX;
    }

    cti->umax = d[resource_name]->umax;
    cti->vmax = d[resource_name]->vmax;
    cti->sprite_count = d[resource_name]->count;
    cti->w = 0;
    cti->h = 0;
    cti->scale = (float)d[resource_name]->scale;

    return CEC_OKAY;
}

int cl_get_static_object_info(int resource_name, cl_static_object_info* csoi)
{
    data::static_object_data d;

    if (csoi == nullptr)
    {
        return CEC_NULL_PARAM;
    }

    if (d[resource_name] == nullptr)
    {
        memset(csoi, 0, sizeof(cl_static_object_info));
        return CEC_INVALID_INDEX;
    }

    strncpy_s(csoi->name, d[resource_name]->name.c_str(), d[resource_name]->name.length());
    strncpy_s(csoi->dds, d[resource_name]->dds.c_str(), d[resource_name]->dds.length());
    csoi->scale = static_cast<float>(d[resource_name]->scale);

    return CEC_OKAY;
}

float cl_get_hp_ratio(int id)
{
    client::entity e(id);
    if (e.is_valid())
    {
        if (ClCtxEm()->has<component::hp>(e))
        {
            int max_hp = ClCtxEm()->get_as<component::hp>(e).max_hp;

            if (max_hp > 0)
                return (float)ClCtxEm()->get_as<component::hp>(e).cur_hp / max_hp;
        }
    }

    return 1.0f;
}

int cl_request_server_list()
{
    using boost::asio::ip::tcp;

    static const char* pylon_server = "xelina.co.kr";
    static const char* pylon_port = "40001";

    try
    {
        tcp::resolver resolver(ClCtx()->io_svc);
        tcp::resolver::query query(pylon_server, pylon_port); // Pylon 포트
        tcp::resolver::iterator iterator = resolver.resolve(query);

        ClCtx()->session.reset(new msg_session(ClCtx()->io_svc));
        ClCtx()->session->connect(iterator);

        cl_start_network_thread();

        msg::request_server_msg msg;

        ClSessionSend(msg);
    }
    catch (...)
    {
        ClDebugOutput("Connection to pylon (%s:%s) failed!", pylon_server, pylon_port);
    }

    return CEC_OKAY;
}

int cl_get_2d_map(float* z, float cellw, float cellh, int pcx, int pcy, float posxOffset, float posyOffset)
{
    static sint OCTAVEARRAY[] = { 1, 1, 1, 1, 1 };

    for (int i = 0; i < pcy; ++i)
    {
        for (int j = 0; j < pcx; ++j)
        {
            float posx = posxOffset + cellw * j;
            float posy = posyOffset + cellh * i;

            cl_get_2d_map_query_height(z++, posx, posy);
        }
    }

    return CEC_OKAY;
}

int cl_get_2d_map_query_height(float* z, float posx, float posy)
{
    *z = QueryProcMapHeight(posx, posy);

    return CEC_OKAY;
}

int cl_player_set_move_speed(float move_speed)
{
    if (ClCtxEm() == nullptr)
        return CEC_CONTEXT_NOT_READY;

    client::get_player_as<component::anim>().move_speed = move_speed;

    return CEC_OKAY;
}

int cl_entity_nameplate_visible(int id)
{
    if (ClCtxEm() == nullptr)
        return CEC_CONTEXT_NOT_READY;

    client::entity e(id);
    if (e.is_valid())
    {
        if (ClCtxEm()->has<component::name>(e))
        {
            return ClCtxEm()->get_as<component::name>(e).nameplate_visible ? 1 : 0;
        }
    }

    return 0;
}

int cl_attach_collider(int id)
{
    return CEC_OKAY;
}

int cl_player_set_pos(float x, float y, float z)
{
    if (ClCtxEm() == nullptr)
        return CEC_CONTEXT_NOT_READY;

    client::get_player_as<component::pos0>().x = x;
    client::get_player_as<component::pos0>().y = y;
    client::get_player_as<component::pos0>().z = z;

    client::get_player_as<component::pos1>().x = x;
    client::get_player_as<component::pos1>().y = y;
    client::get_player_as<component::pos1>().z = z;

    return CEC_OKAY;
}

int cl_peek_msg_from_test_session(msg_type_t* msg_type, msg_reader* reader)
{
    if (ClCtxEm() == nullptr)
        return CEC_CONTEXT_NOT_READY;

    ClCtx()->io_svc.poll();

    msg_session_ref session = ClCtx()->session;
    test_msg_session* session_ptr = dynamic_cast<test_msg_session*>(session.get());
     if (session_ptr == nullptr)
         return CEC_NO_NETWORK_SESSION;

     if (session_ptr->is_queue_empty())
         return CEC_EMPTY;

     auto pair = session_ptr->peek_msg();
     if (!pair.second.is_valid())
         return CEC_EMPTY;

     *msg_type = pair.first;
     *reader = pair.second;
    return CEC_OKAY;
}

int cl_pop_msg_from_test_session()
{
    if (ClCtxEm() == nullptr)
        return CEC_CONTEXT_NOT_READY;

    msg_session_ref session = ClCtx()->session;
    test_msg_session* session_ptr = dynamic_cast<test_msg_session*>(session.get());
     if (session_ptr == nullptr)
         return CEC_NO_NETWORK_SESSION;

     if (session_ptr->is_queue_empty())
         return CEC_EMPTY;

     session_ptr->pop_msg();
    return CEC_OKAY;
}

int cl_get_player_pos0(float* p)
{
    auto& c = client::get_player_as<component::pos0>();
    p[0] = (float)c.x;
    p[1] = (float)c.y;
    p[2] = (float)c.z;

    return CEC_OKAY;
}

int cl_get_player_pos0d(float* p)
{
    auto& c = client::get_player_as<component::pos0>();
    p[0] = (float)c.dx;
    p[1] = (float)c.dy;
    p[2] = (float)c.dz;

    return CEC_OKAY;
}

int cl_get_player_pos1(float* p)
{
    auto& c = client::get_player_as<component::pos1>();
    p[0] = (float)c.x;
    p[1] = (float)c.y;
    p[2] = (float)c.z;

    return CEC_OKAY;
}

int cl_poll_network_service()
{
    ClCtx()->io_svc.poll();
    return CEC_OKAY;
}
