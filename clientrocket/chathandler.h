#pragma once

#include "EventHandler.h"

class ChatHandler : public EventHandler
{
public:
	ChatHandler();
	virtual ~ChatHandler();

	virtual void ProcessEvent(Rocket::Core::Event& event, const Rocket::Core::String& value);

private:
    void DoChat(Rocket::Core::Event& event);
};
