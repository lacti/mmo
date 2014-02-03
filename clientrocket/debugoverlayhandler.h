#pragma once

#include "EventHandler.h"

class DebugOverlayHandler : public EventHandler
{
public:
	DebugOverlayHandler();
	virtual ~DebugOverlayHandler();

	virtual void ProcessEvent(Rocket::Core::Event& event, const Rocket::Core::String& value);
};
