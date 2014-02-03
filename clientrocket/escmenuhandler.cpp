#include "stdafx.h"
#include "EscMenuHandler.h"
#include "EventManager.h"
#include "../clientlogic/clientlogic.h"
#include <Rocket/Core/Context.h>

extern Rocket::Core::Context* gContext;

EscMenuHandler::EscMenuHandler()
{
}

EscMenuHandler::~EscMenuHandler()
{
}

void EscMenuHandler::ProcessEvent(Rocket::Core::Event& event, const Rocket::Core::String& value)
{
    if (value == "logout")
    {   
        cl_disconnect_from_server();
        cl_stop_network_thread();

        while (auto pDoc = gContext->GetDocument("modalwait"))
        {
            pDoc->Close();
        }
        while (auto pDoc = gContext->GetDocument("chat"))
        {
            pDoc->Close();
        }
        while (auto pDoc = gContext->GetDocument("escmenu"))
        {
            pDoc->Close();
        }

        EventManager::LoadWindow("background");
    }
    else if (value == "closeapp")
    {
        extern void RequestCloseApp();

        RequestCloseApp();
    }
    else if (value == "closemenu")
    {
        event.GetCurrentElement()->GetOwnerDocument()->Close();
    }
}
