#pragma once

void InitializeRocket();
void UpdateRenderRocket(int w, int h);
Rocket::Core::Input::KeyIdentifier TranslateSDLKey(SDL_Keycode key);
void FinalizeRocket();
