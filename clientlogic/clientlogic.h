#pragma once

#ifndef CLIENTLOGIC_API
#	ifdef CLIENTLOGIC_EXPORTS
#		define CLIENTLOGIC_API __declspec(dllexport)
#	else
#		define CLIENTLOGIC_API __declspec(dllimport)
#	endif
#endif

class msg_reader;
typedef int    msg_type_t;

enum cl_error_code
{
    CEC_OKAY,
	CEC_CONTEXT_NOT_READY,
    CEC_NULL_PARAM,
    CEC_DUPLICATED_REQUEST,
    CEC_DUPLICATED_MNAME,
    CEC_NO_NETWORK_SESSION,
    CEC_INVALID_ENTITY,
	CEC_EVENT_EMPTY,
	CEC_INVALID_INDEX,
    CEC_EMPTY,
    CEC_MAX
};

enum cl_atomic_command
{
    CAC_INVALID,
    CAC_ENTITY_MOVE_LEFT,
    CAC_ENTITY_MOVE_RIGHT,
    CAC_ENTITY_MOVE_UP,
    CAC_ENTITY_MOVE_DOWN,
    CAC_ENTITY_MOVE_STOP,
    CAC_MAX
};

enum cl_game_event
{
    CGE_INVALID,
    CGE_PLAYER_SPAWNED,
    CGE_MAX
};

typedef void (*cl_fn_debug_output_handler)(const char* line);
typedef void (*cl_fn_chat_output_handler)(int id, const char* line);
typedef void (*cl_fn_game_event_handler)(int event_type);
typedef void (*cl_fn_event_pushed_event_handler)();
typedef size_t mmocl_context;

typedef struct cl_visible_entity
{
	int id;
    double x, y, z;
    int resource_name;
    int uindex, vindex;
    int is_static_object;
} cl_visible_entity;

typedef struct cl_tex_info
{
	int w, h;
	int umax, vmax;
	int sprite_count;
	float scale;
} cl_tex_info;

typedef struct cl_static_object_info
{
	char name[64];
    char dds[64];
    float scale;
} cl_static_object_info;

typedef struct cl_initialize_params
{
    void (*debug_line_output_handler)(const char* line);
    void (*chat_output_handler)(int id, const char* line);
    void (*game_event_handler)(int event_type);
	void (*event_pushed_event_handler)();
} cl_initialize_params;

#ifdef __cplusplus
extern "C"
{
#endif

typedef int (*spawn_callback_type)(int id, double x, double y);
typedef int (*world_info_callback_type)(const char* world_name);
typedef int (*alert_callback_type)(const char* message);
typedef int (*voice_callback_type)(const char* buffer, size_t buflen);
typedef int (*update_hp_callback_type)();

CLIENTLOGIC_API int cl_test_func(int v);
CLIENTLOGIC_API int cl_initialize_context(mmocl_context* ctx, cl_initialize_params* params);
CLIENTLOGIC_API int cl_bind_context(mmocl_context ctx);
CLIENTLOGIC_API int cl_destroy_context(void);
CLIENTLOGIC_API int cl_player_pos(double* x, double* y);
CLIENTLOGIC_API int cl_player_pos_z(double* z);
CLIENTLOGIC_API int cl_entity_pos(double* x, double* y, int id);
CLIENTLOGIC_API int cl_entity_pos_z(double* z, int id);
CLIENTLOGIC_API int cl_entity_command(int com, int id);
CLIENTLOGIC_API const char* cl_entity_name(int id);
CLIENTLOGIC_API int cl_frame_move(double dt);
CLIENTLOGIC_API int cl_get_player_anim_index(int* u, int* v);
CLIENTLOGIC_API const char* cl_player_name();
CLIENTLOGIC_API int cl_connect_to_server(const char* server, bool for_test = false);
CLIENTLOGIC_API int cl_disconnect_from_server();
CLIENTLOGIC_API int cl_start_network_thread();
CLIENTLOGIC_API int cl_stop_network_thread();
CLIENTLOGIC_API int cl_enter_world(const char* name); // 로그인 요청이 발생했을 때 호출한다.
CLIENTLOGIC_API int cl_player_chat(const char* line);
CLIENTLOGIC_API int cl_player_id();
CLIENTLOGIC_API int cl_is_player_valid();
CLIENTLOGIC_API int cl_visible_entity_count();
CLIENTLOGIC_API int cl_visible_entities(cl_visible_entity* out_entities, int max_count);
CLIENTLOGIC_API int cl_list_sprites();
CLIENTLOGIC_API int cl_character_sprite_count();
CLIENTLOGIC_API const char* cl_sprite_name(int sprite_id);
CLIENTLOGIC_API const char* cl_world_name(int world_id);
CLIENTLOGIC_API int cl_interact();
CLIENTLOGIC_API int cl_interact_ex(int interact_id);
CLIENTLOGIC_API int cl_interact_heal();
CLIENTLOGIC_API int cl_interact_attack();
CLIENTLOGIC_API int cl_py_initialize(const char* server, const char* user);
CLIENTLOGIC_API int cl_py_set_spawn_callback(spawn_callback_type fcb);
CLIENTLOGIC_API int cl_py_set_world_info_callback(world_info_callback_type fcb);
CLIENTLOGIC_API int cl_py_set_alert_callback(alert_callback_type fcb);
CLIENTLOGIC_API int cl_py_set_voice_callback(voice_callback_type fcb);
CLIENTLOGIC_API int cl_py_set_update_hp_callback(update_hp_callback_type fcb);
CLIENTLOGIC_API void cl_debug_output(const char* format, ...);
CLIENTLOGIC_API int cl_spawn_entity();
CLIENTLOGIC_API int cl_spawn_fireball(int owner_entity_id);
CLIENTLOGIC_API int cl_spawn_cylinder();
CLIENTLOGIC_API int cl_get_tex_info(int resource_name, cl_tex_info* cti);
CLIENTLOGIC_API int cl_get_static_object_info(int resource_name, cl_static_object_info* csoi);
CLIENTLOGIC_API float cl_get_hp_ratio(int id);
CLIENTLOGIC_API int cl_request_server_list();
CLIENTLOGIC_API int cl_get_2d_map(float* z, float cellw, float cellh, int pcx, int pcy, float posxOffset, float posyOffset);
CLIENTLOGIC_API int cl_get_2d_map_query_height(float* z, float posx, float posy);
CLIENTLOGIC_API int cl_player_set_move_speed(float move_speed);
CLIENTLOGIC_API int cl_player_set_pos(float x, float y, float z);
CLIENTLOGIC_API int cl_entity_nameplate_visible(int id);
CLIENTLOGIC_API int cl_get_logicworld_ghost_pos(float* pos);
CLIENTLOGIC_API int cl_set_logicworld_ghost_left(bool b);
CLIENTLOGIC_API int cl_set_logicworld_ghost_right(bool b);
CLIENTLOGIC_API int cl_set_logicworld_ghost_up(bool b);
CLIENTLOGIC_API int cl_set_logicworld_ghost_down(bool b);
CLIENTLOGIC_API int cl_set_logicworld_ghost_jump(bool b);
CLIENTLOGIC_API int cl_peek_msg_from_test_session(msg_type_t* msg_type, msg_reader* reader);
CLIENTLOGIC_API int cl_pop_msg_from_test_session();
CLIENTLOGIC_API int cl_get_player_pos0(float* p);
CLIENTLOGIC_API int cl_get_player_pos0d(float* p);
CLIENTLOGIC_API int cl_get_player_pos1(float* p);
CLIENTLOGIC_API int cl_poll_network_service();

#ifdef __cplusplus
}
#endif
