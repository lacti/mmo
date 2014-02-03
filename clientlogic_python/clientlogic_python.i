%module clientlogic
%include "typemaps.i"

%{
#include "clientlogic.h"
%}

struct cl_visible_entity
{
    double x, y, z;
    int tex_mname;
    int uindex, vindex;
};

struct cl_initialize_params
{
    void (*debug_line_output_handler)(const char* line);
    void (*chat_output_handler)(int id, const char* line);
    void (*game_event_handler)(int event_type);
};

typedef size_t mmocl_context;

%apply mmocl_context *OUTPUT { mmocl_context *ctx };

extern int cl_initialize_context(mmocl_context* ctx, cl_initialize_params* params);
extern int cl_connect_to_server(const char* server);
extern int cl_enter_world(const char* name);
extern int cl_start_network_thread();
extern int cl_destroy_context();
