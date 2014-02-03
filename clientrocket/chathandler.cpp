#include "stdafx.h"
#include "chathandler.h"
#include "EventManager.h"
#include "../clientlogic/clientlogic.h"

ChatHandler::ChatHandler()
{
}

ChatHandler::~ChatHandler()
{
}

void ChatHandler::DoChat(Rocket::Core::Event& event)
{
    auto chatline = event.GetTargetElement()->GetElementById("chatline");
    if (chatline)
    {
        auto line = chatline->GetAttribute<Rocket::Core::String>("value", "");
        if (line.Length() > 0)
        {
            cl_player_chat(line.CString());
        }
    }
    else
    {
        // ASSERT
    }
    
}

Rocket::Core::Element* gChatLogField = nullptr;

void ChatHandler::ProcessEvent(Rocket::Core::Event& event, const Rocket::Core::String& value)
{
    if (value == "load_chat")
    {   
        gChatLogField = event.GetTargetElement()->GetElementById("chatlog");
    }

    if (value == "enter_key")
    {
        if (event.GetParameter< int >("key_identifier", Rocket::Core::Input::KI_UNKNOWN) == Rocket::Core::Input::KI_RETURN)
        {
            DoChat(event);
        }
    }
}
