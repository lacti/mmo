#pragma once

#ifndef CLIENTLOGIC_API
#	ifdef CLIENTLOGIC_EXPORTS
#		define CLIENTLOGIC_API __declspec(dllexport)
#	else
#		define CLIENTLOGIC_API __declspec(dllimport)
#	endif
#endif

enum mmocl_event_type
{
    MET_INVALID,
    MET_ENTER_WORLD,
    MET_CONNECT_FAIL,
    MET_SERVERS,
    MET_SERVER,
};

typedef struct cl_enter_world_event
{
    int type;
    int world_id;
} cl_enter_world_event;

typedef struct cl_server_event
{
    int type;
    int port;
    char name[64];
    char host[64];
} cl_server_event;

typedef union mmocl_event
{
    int type;
    cl_enter_world_event enter_world;
    cl_server_event server;
} mmocl_event;


CLIENTLOGIC_API int cl_poll_event(mmocl_event* event);
