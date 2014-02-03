#include "stdafx.h"
#include "DebugOverlayHandler.h"
#include "EventManager.h"
#include "../clientlogic/clientlogic.h"

DebugOverlayHandler::DebugOverlayHandler()
{
}

DebugOverlayHandler::~DebugOverlayHandler()
{
}

Rocket::Core::Element* gFpsField = nullptr;
Rocket::Core::Element* gPos0Field = nullptr;
Rocket::Core::Element* gPos1Field = nullptr;

void DebugOverlayHandler::ProcessEvent(Rocket::Core::Event& event, const Rocket::Core::String& value)
{
    if (value == "load_debug_overlay")
    {   
        gFpsField = event.GetTargetElement()->GetElementById("fps");
        gPos0Field = event.GetTargetElement()->GetElementById("pos0");
        gPos1Field = event.GetTargetElement()->GetElementById("pos1");
    }
}
