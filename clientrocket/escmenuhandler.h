#pragma once

#include "EventHandler.h"

class EscMenuHandler : public EventHandler
{
public:
	EscMenuHandler();
	virtual ~EscMenuHandler();

	virtual void ProcessEvent(Rocket::Core::Event& event, const Rocket::Core::String& value);
};
