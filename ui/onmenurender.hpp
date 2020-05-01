//
// DO NOT INCLUDE THIS FILE
//

#pragma once
#include "../imgui/imgui.h"

#include "../features/playerlist.hpp"
#include "../features/esp.hpp"
#include "../features/namechanger.hpp"
#include "../features/customdisconnect.hpp"
#include "../features/gmod/lualoader.hpp"
#include "../shutdown.hpp"

#include "../config/config.hpp"

void OnMenuRender()
{
	Main::DrawMenu();
	Features::PlayerList::DrawMenu();
	Features::Esp::DrawEntityListMenu();
	Features::NameChanger::DrawMenu();
	Features::CustomDisconnect::DrawMenu();
	#if SOURCE_SDK_IS_GMOD
	Features::GarrysMod::LuaLoader::DrawMenu();
	#endif

	Config::DrawMenu();
}
