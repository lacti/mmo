/*
* This source file is part of libRocket, the HTML/CSS Interface Middleware
*
* For the latest information, see http://www.librocket.com
*
* Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*
*/
#include "stdafx.h"
#include "EventHandlerBackground.h"
#include <Rocket/Core/ElementDocument.h>
#include <Rocket/Core/ElementUtilities.h>
#include <Rocket/Core/Input.h>
#include "EventManager.h"

extern void RequestCloseApp();

EventHandlerBackground::EventHandlerBackground()
{
}

EventHandlerBackground::~EventHandlerBackground()
{
}

void SetUserList(std::vector<std::string> list);
void Login(const char* username, const char* worldname);

void EventHandlerBackground::DoConnect(const Rocket::Core::Event& event) const
{
    auto username_field = event.GetTargetElement()->GetElementById("username");
    auto world_field = event.GetTargetElement()->GetElementById("world");
    if (username_field && world_field)
    {
        auto username = username_field->GetAttribute<Rocket::Core::String>("value", "");
        auto world = world_field->GetAttribute<Rocket::Core::String>("value", "localhost");

        Login(username.CString(), world.CString());

        if (EventManager::LoadWindow("modalwait"))
            event.GetTargetElement()->GetOwnerDocument()->Close();
    }
    else
    {
        // ASSERT
    }
}

void EventHandlerBackground::ProcessEvent(Rocket::Core::Event& event, const Rocket::Core::String& value)
{
    //OutputDebugStringA(value.CString());
    //OutputDebugStringA("\n");

    if (value == "connect")
    {
        DoConnect(event);
    }
    else if (value == "closeapp")
    {
        RequestCloseApp();
    }
    else if (value == "add_score")
    {

    }
    else if (value == "enter_name")
    {
        if (event.GetParameter< int >("key_identifier", Rocket::Core::Input::KI_UNKNOWN) == Rocket::Core::Input::KI_RETURN)
        {
            //Rocket::Core::String name = event.GetCurrentElement()->GetAttribute< Rocket::Core::String >("value", "Anon.");

            DoConnect(event);

        }
    }
    else if (value == "check_input")
    {
        Rocket::Core::Element* name_input_field = event.GetTargetElement()->GetElementById("player_input");
        if (name_input_field)
        {
            name_input_field->Focus();
        }
    }
    else if (value == "check_name")
    {
        /* TODO: Check if the player hasn't set their name first. */
        std::vector<std::string> list;
        SetUserList(list);
    }
    else if (value == "messagebox")
    {
        MessageBoxA(NULL, "OH", "CAPTION", MB_OK);
    }
}
