#include "stdafx.h"
#include "ServerListHandler.h"
#include "EventManager.h"
#include "../clientlogic/clientlogic.h"

ServerListHandler::ServerListHandler()
{
}

ServerListHandler::~ServerListHandler()
{
}

void ServerListHandler::ProcessEvent(Rocket::Core::Event& event, const Rocket::Core::String& value)
{
    if (value == "load_server_list")
    {
        cl_request_server_list();
    }
}
