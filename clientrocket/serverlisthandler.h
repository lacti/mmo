#pragma once

#include "EventHandler.h"

class ServerListHandler : public EventHandler
{
public:
	ServerListHandler();
	virtual ~ServerListHandler();

	virtual void ProcessEvent(Rocket::Core::Event& event, const Rocket::Core::String& value);
};
