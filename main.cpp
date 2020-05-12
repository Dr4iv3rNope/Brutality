#define NOMINMAX
#include "main.hpp"
#include "shutdown.hpp"


#include "util/httpclient.hpp"
#include "util/debug/logs.hpp"
#include "util/debug/labels.hpp"
#include "util/debug/winapi.hpp"
#include "util/debug/errors.hpp"

#include "valvesdk/interfaces.hpp"

#include "steamapi/steamuser.hpp"
#include "steamapi/steamclient.hpp"
#include "steamapi/steaminterfaces.hpp"

// helpers
#include "imgui/custom/windowmanager.hpp"
#include "sourcesdk/netvars.hpp"
#include "sourcesdk/clientclassdumper.hpp"

#include "features/namechanger.hpp"
#include "features/playerlist.hpp"
#include "features/customdisconnect.hpp"
#include "features/esp.hpp"
#include "features/chams.hpp"
#include "features/chatspam.hpp"
#include "gmod/features/lualoader.hpp"
#include "gmod/features/antiscreengrab.hpp"

#include "config/config.hpp"
#include "config/variable.hpp"


#include <thread>
#include <list>
#include <atomic>
#include <stdexcept>
#include <filesystem>
#include <mutex>

#include <Windows.h>
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")

#undef CreateEvent

#define __INLINE_RUN_FUNC(name) \
	class _##name final { \
		public: inline _##name() noexcept; \
	} __irf##name; inline _##name::_##name() noexcept
	

//
// implementing local path system
//

static std::wstring localPath;

bool Main::IsLocalPathInitialized()
{
	return !localPath.empty();
}

const std::wstring& Main::GetLocalPath()
{
	if (!IsLocalPathInitialized())
	{
		wchar_t* folderPath;

		if (SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DONT_VERIFY, nullptr, &folderPath) != S_OK)
		{
			// prevent infinity loop/stack overflow
			UTIL_ASSERT_MSGBOX(Util::Debug::LogSystem::IsInitialized(), L"failed to get documents folder");

			UTIL_RUNTIME_ERROR("failed to get documents folder");
		}

		localPath = std::wstring(folderPath);
		localPath += UTIL_SXOR(L"\\Brutality\\");

		if (!std::filesystem::exists(localPath))
		{
			if (!std::filesystem::create_directory(localPath))
			{
				// prevent infinity loop/stack overflow
				UTIL_ASSERT_MSGBOX(Util::Debug::LogSystem::IsInitialized(), L"failed to get documents folder");

				UTIL_RUNTIME_ERROR("failed to create hack folder");
			}
		}

		CoTaskMemFree(folderPath);
	}

	return localPath;
}


namespace
{
	__INLINE_RUN_FUNC(InitLogs)
	{
		Util::Debug::LogSystem::Initialize(Util::Debug::LogSystem::GetLogFilePath());
	}
}

#pragma region Declare game classes

GameInterfaces* interfaces;
GameHooks* hooks;
SteamInterfaces* steamInterfaces;

#pragma endregion

#pragma region Initialize helpers

SourceSDK::NetvarManager* SourceSDK::netvars;
SourceSDK::ClientClassDumper* SourceSDK::clientClassDumper;

ImGui::Custom::WindowManager* ImGui::Custom::windowManager;

#pragma endregion

#pragma region Initialize config variables

#if BUILD_GAME_IS_GMOD
// anti-screengrab

Config::Bool antiScreenGrabEnable(UTIL_SXOR("Protection"), UTIL_SXOR("Anti-Screen Grab"));


// run string

Config::Bool preventRunString(UTIL_SXOR("Protection"), UTIL_SXOR("Prevent Run String"));
#endif

#ifdef _DEBUG
// debug variables

static Config::Bool v_bool("Debug", "Boolean", Config::VariableFlags_DontSave);
static Config::Int32 v_int32("Debug", "Integer 32", Config::VariableFlags_DontSave);
static Config::UInt32 v_uint32("Debug", "Unsigned Integer 32", Config::VariableFlags_DontSave);
static Config::Float v_float("Debug", "Float", Config::VariableFlags_DontSave);

static Config::LInt32 lv_int32("Debug", "Limited Integer 32", 0, -228, 1337, Config::VariableFlags_DontSave);
static Config::LUInt32 lv_uint32("Debug", "Limited Unsigned Integer 32", 666, 666, 1234, Config::VariableFlags_DontSave);
static Config::LFloat lv_float("Debug", "Limited Float", 0, -3.14f, 2.28f, Config::VariableFlags_DontSave);
static Config::LimitedString<char> lv_string("Debug", "Your Name", 64, Config::VariableFlags_DontSave, "AYE");

static Config::String<char> uv_string("Debug", "XYZ", Config::VariableFlags_DontSave, "123");
static Config::Enum uv_enum("Debug", "Who are you", { "Idiot", "I Have Stupid", "Yes sir" }, Config::VariableFlags_DontSave);
static Config::Color uv_color("Debug", "Pick some color", 0.1f, 0.3f, 0.3f, 0.7f, Config::VariableFlags_DontSave);
static Config::Key uv_key("Debug", "Some Key Binding", Config::VariableFlags_DontSave);
static Config::Flags uv_flags("Debug", "Flags", { "flag 1", "flag 2", "pride flag", "flag 3" }, Config::VariableFlags_DontSave);
#endif

Config::Color playerColorNormal(UTIL_SXOR("Player Colors"), UTIL_SXOR("Normals Color"), { 255, 255, 255, 255 });
Config::Color playerColorDangerous(UTIL_SXOR("Player Colors"), UTIL_SXOR("Dangerouses Color"), { 255, 0, 0, 155 });
Config::Color playerColorFriends(UTIL_SXOR("Player Colors"), UTIL_SXOR("Friends Color"), { 55, 255, 55, 255 });
Config::Color playerColorRages(UTIL_SXOR("Player Colors"), UTIL_SXOR("Rages Color"), { 255, 100, 0, 255 });


// esp

Config::Bool espEnabled(UTIL_SXOR("ESP"), UTIL_SXOR("Enable"), false);
Config::LFloat espMaxDistance(UTIL_SXOR("ESP"), UTIL_SXOR("Max Distance"), 0.f, 0.f, 50000.f);

Config::Bool espUpdatePerFrame(UTIL_SXOR("ESP"), UTIL_SXOR("Update Per Frame"), false);

Config::Bool espDrawEntities(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Entities"), false);
Config::Bool espDrawNormal(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Normal"), false);
Config::Bool espDrawDangerous(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Dangerous"), false);
Config::Bool espDrawFriends(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Friends"), false);
Config::Bool espDrawRages(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Rages"), false);
Config::Bool espDrawDeadPlayers(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Dead Players"), false);
Config::Bool espDrawDormantPlayers(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Dormant Players"), false);

Config::Bool espHealth(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Health"), false);
Config::Color espHealthColor(UTIL_SXOR("ESP"), UTIL_SXOR("Health Text Color"), { 55, 255, 55, 255 });

Config::Bool espName(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Name"), false);
Config::Color espNameColor(UTIL_SXOR("ESP"), UTIL_SXOR("Name Text Color"), { 55, 255, 255, 255 });

Config::Bool espSkeleton(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Skeleton"), false);
#ifdef _DEBUG
Config::Bool dbg_espSkeleton(UTIL_SXOR("ESP"), UTIL_SXOR("Debug Skeleton"), false);
#endif
Config::LFloat espSkeletonThickness(UTIL_SXOR("ESP"), UTIL_SXOR("Skeleton Thickness"), 1.f, 0.1f, 12.f);

Config::Bool espActiveWeapon(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Active Weapon"), false);
Config::Color espActiveWeaponColor(UTIL_SXOR("ESP"), UTIL_SXOR("Active Weapon Text Color"), { 55, 55, 55, 255 });


// text radar

Config::Bool textRadarEnable(UTIL_SXOR("Text Radar"), UTIL_SXOR("Enable"), false);

Config::UInt32 textRadarX(UTIL_SXOR("Text Radar"), UTIL_SXOR("X"), 0);
Config::UInt32 textRadarY(UTIL_SXOR("Text Radar"), UTIL_SXOR("Y"), 0);

Config::LUInt32 textRadarMaxPlayers(UTIL_SXOR("Text Radar"), UTIL_SXOR("Max Players"), 10, 5, 32);
Config::LFloat textRadarMaxDistance(UTIL_SXOR("Text Radar"), UTIL_SXOR("Max Distance"), 0.f, 0.f, 50000.f);

Config::Bool textRadarDrawDistance(UTIL_SXOR("Text Radar"), UTIL_SXOR("Draw Distance"), false);


// spectator list
Config::Bool spectatorListEnable(UTIL_SXOR("Spectator List"), UTIL_SXOR("Enable"));

Config::UInt32 spectatorListX(UTIL_SXOR("Spectator List"), UTIL_SXOR("X"), 0);
Config::UInt32 spectatorListY(UTIL_SXOR("Spectator List"), UTIL_SXOR("Y"), 0);

Config::LUInt32 spectatorListMaxPlayers(UTIL_SXOR("Spectator List"), UTIL_SXOR("Max Players"), 5, 2, 32);


// movement
Config::Bool fastWalkEnable(UTIL_SXOR("Movement"), UTIL_SXOR("Fast Walk"));


// third person
//Config::Bool thirdPersonEnable(true, UTIL_SXOR("Third Person"), UTIL_SXOR("Enable"), false);
//Config::Bool thirdPersonCameraCollide(true, UTIL_SXOR("Third Person"), UTIL_SXOR("Collideable Camera"), true);
//Config::Key thirdPersonSwitchKey(true, UTIL_SXOR("Third Person"), UTIL_SXOR("Toggle Key"), ImGui::Custom::Keys::ALT);
//
//Config::LFloat thirdPersonX(true, UTIL_SXOR("Third Person"), UTIL_SXOR("X"), -100.f, -500.f, 500.f);
//Config::LFloat thirdPersonY(true, UTIL_SXOR("Third Person"), UTIL_SXOR("Y"), 0.f, -200.f, 200.f);
//Config::LFloat thirdPersonZ(true, UTIL_SXOR("Third Person"), UTIL_SXOR("Z"), 0.f, -200.f, 200.f);


// crosshair

Config::Bool crosshairEnable(UTIL_SXOR("Crosshair"), UTIL_SXOR("Enable"));

Config::LUInt32 crosshairGap(UTIL_SXOR("Crosshair"), UTIL_SXOR("Gap"), 0, 0, 150);
Config::LUInt32 crosshairSize(UTIL_SXOR("Crosshair"), UTIL_SXOR("Size"), 1, 1, 150);
Config::LUInt32 crosshairThickness(UTIL_SXOR("Crosshair"), UTIL_SXOR("Thickness"), 1, 1, 30);

Config::Color crosshairColor(UTIL_SXOR("Crosshair"), UTIL_SXOR("Color"), { 255, 255, 255, 255 });


// bunny hop

Config::Bool bhopEnable(UTIL_SXOR("Bunny Hop"), UTIL_SXOR("Enable"));
Config::Bool bhopAutoStrafe(UTIL_SXOR("Bunny Hop"), UTIL_SXOR("Mouse Auto-Strafe"));

/*
Config::LFloat bhopMinDelay(UTIL_SXOR("Bunny Hop"), UTIL_SXOR("Min Jump Delay"), 0.f, 0.f, 1000.f);
Config::LFloat bhopMaxDelay(UTIL_SXOR("Bunny Hop"), UTIL_SXOR("Max Jump Delay"), 0.f, 0.f, 1000.f);
*/


// chat spam

Config::Bool chatSpamEnable(
	UTIL_SXOR("Chat Spammer"),
	UTIL_SXOR("Enable"),
	Config::VariableFlags_NotVisible
);

Config::Bool chatSpamTeam(
	UTIL_SXOR("Chat Spammer"),
	UTIL_SXOR("Team chat mode"),
	Config::VariableFlags_NotVisible
);

Config::LFloat chatSpamDelay(
	UTIL_SXOR("Chat Spammer"),
	UTIL_SXOR("Delay in seconds"),
	0.f, 0.f, 3.f,
	Config::VariableFlags_NotVisible
);

Config::String<char> chatSpamMessage(
	UTIL_SXOR("Chat Spammer"),
	UTIL_SXOR("Message"),
	Config::VariableFlags_NotVisible,
	UTIL_SXOR("\\n\\nBrutality B-)\\n\\n")
);

Config::Enum chatSpamMode(
	UTIL_SXOR("Chat Spammer"),
	UTIL_SXOR("Mode"),
	{
		UTIL_SXOR("Message"),
		UTIL_SXOR("Animated")
	},
	Config::VariableFlags_NotVisible
);


// user interface settings

Config::Bool useTabs(UTIL_SXOR("Interface"), UTIL_SXOR("Use Tabs instead of windows"), Config::VariableFlags_NotVisible, true);


// name changer

Config::String<char> nameChangerName(UTIL_SXOR("Name Changer"), UTIL_SXOR("Name"), Config::VariableFlags_NotVisible);
Config::Bool nameChangerAutoSend(UTIL_SXOR("Name Changer"), UTIL_SXOR("Auto Send"), Config::VariableFlags_NotVisible);


// chams

Config::Bool chamsEnable(UTIL_SXOR("Chams"), UTIL_SXOR("Enable"));
Config::Bool chamsOnlyVisible(UTIL_SXOR("Chams"), UTIL_SXOR("Only Visible"));
Config::Bool chamsDrawDormant(UTIL_SXOR("Chams"), UTIL_SXOR("Draw Dormant"));

std::initializer_list chamsTypes =
{
	UTIL_SXOR("Disable"),
	UTIL_SXOR("Normal"),
	UTIL_SXOR("Flat"),
	UTIL_SXOR("Shiny"),
	UTIL_SXOR("Glow"),
	UTIL_SXOR("Animated Spawn Effect"),
	UTIL_SXOR("Glass")
};

Config::Color chamsNormalsVisibleColor(UTIL_SXOR("Chams"), UTIL_SXOR("Visible Normal Players Color"), { 255, 255, 255, 255 });
Config::Color chamsNormalsColor(UTIL_SXOR("Chams"), UTIL_SXOR("Normal Players Color"), { 255, 255, 155, 255 });
Config::Enum chamsNormalsType(UTIL_SXOR("Chams"), UTIL_SXOR("Normal Players"), chamsTypes);

Config::Color chamsDangerousVisibleColor(UTIL_SXOR("Chams"), UTIL_SXOR("Visible Dangerous Players Color"), { 255, 0, 0, 155 });
Config::Color chamsDangerousColor(UTIL_SXOR("Chams"), UTIL_SXOR("Dangerous Players Color"), { 155, 55, 55, 155 });
Config::Enum chamsDangerousType(UTIL_SXOR("Chams"), UTIL_SXOR("Dangerous Players"), chamsTypes);

Config::Color chamsFriendsVisibleColor(UTIL_SXOR("Chams"), UTIL_SXOR("Visible Friend Players Color"), { 55, 255, 55, 255 });
Config::Color chamsFriendsColor(UTIL_SXOR("Chams"), UTIL_SXOR("Friend Players Color"), { 100, 155, 55, 255 });
Config::Enum chamsFriendsType(UTIL_SXOR("Chams"), UTIL_SXOR("Friend Players"), chamsTypes);

Config::Color chamsRagesVisibleColor(UTIL_SXOR("Chams"), UTIL_SXOR("Visible Rage Players Color"), { 255, 100, 0, 255 });
Config::Color chamsRagesColor(UTIL_SXOR("Chams"), UTIL_SXOR("Rage Players Color"), { 200, 55, 55, 255 });
Config::Enum chamsRagesType(UTIL_SXOR("Chams"), UTIL_SXOR("Rage Players"), chamsTypes);


Config::Color chamsArmsColor(UTIL_SXOR("Chams"), UTIL_SXOR("Arms Color"), 1.f, 1.f, 1.f);
Config::Enum chamsArmsType(UTIL_SXOR("Chams"), UTIL_SXOR("Arms"), chamsTypes);

Config::Color chamsWeaponsColor(UTIL_SXOR("Chams"), UTIL_SXOR("Weapons Color"), 1.f, 1.f, 1.f);
Config::Enum chamsWeaponsType(UTIL_SXOR("Chams"), UTIL_SXOR("Weapons"), chamsTypes);

#pragma endregion

#pragma region Initialize hack

static void InitializeHelpers() noexcept
{
	SourceSDK::netvars = new SourceSDK::NetvarManager();
	SourceSDK::clientClassDumper = new SourceSDK::ClientClassDumper();

	ImGui::Custom::windowManager = new ImGui::Custom::WindowManager();
}

static void InitializeFeatures() noexcept
{
	Features::NameChanger::Initialize();
	Features::Chams::Initialize();
	#if BUILD_GAME_IS_GMOD
	GarrysMod::Features::AntiScreenGrab::Initialize();
	#endif
}

static void RegisterWindows() noexcept
{
	Features::NameChanger::RegisterWindow();
	Features::Esp::RegisterEntityListWindow();
	Features::PlayerList::RegisterWindow();
	Features::ChatSpam::RegisterWindow();
	Features::CustomDisconnect::RegisterWindow();
	#if BUILD_GAME_IS_GMOD
	GarrysMod::Features::LuaLoader::RegisterWindow();
	#endif

	Config::RegisterVariablesInWindowManager();
	Config::RegisterWindow();
}

#ifdef _DEBUG
static bool dbg_initialized { false };
#endif

void Main::Initialize() noexcept
{
	UTIL_DEBUG_ASSERT(!dbg_initialized);

	::interfaces = new GameInterfaces();

	InitializeHelpers();
	InitializeFeatures();	
	RegisterWindows();

	::hooks = new GameHooks();

	#ifdef _DEBUG
	dbg_initialized = true;
	#endif
}

namespace
{
	__INLINE_RUN_FUNC(_Initialize)
	{
		Main::Initialize();
	}
}

#pragma endregion

//
// shutdown
//
static std::atomic_bool isInShutdown { false };

static inline auto& GetShutdownItemsMutex() noexcept
{
	static std::mutex shutdownItemsMutex {};

	return shutdownItemsMutex;
}

static inline auto& GetShutdownItems() noexcept
{
	static std::list<Shutdown::Element*> shutdownItems {};

	return shutdownItems;
}

void Main::AddToShutdown(Shutdown::Element* element) noexcept
{
	UTIL_DEBUG_ASSERT(element);

	GetShutdownItemsMutex().lock();

	UTIL_DEBUG_ASSERT(
		std::find(GetShutdownItems().begin(), GetShutdownItems().end(), element)
		==
		GetShutdownItems().end()
	);

	GetShutdownItems().push_back(element);
	GetShutdownItemsMutex().unlock();
}

void Main::Shutdown() noexcept
{
	UTIL_DEBUG_ASSERT(dbg_initialized);
	UTIL_DEBUG_ASSERT(!isInShutdown);

	isInShutdown = true;

	static std::thread shutdownThread([] ()
	{
		UTIL_LABEL_ENTRY(UTIL_XOR(L"Shutdown"));

		GetShutdownItemsMutex().lock();

		while (!GetShutdownItems().empty())
		{
			std::remove_if(GetShutdownItems().begin(), GetShutdownItems().end(), [] (Shutdown::Element* element)
			{
				if (!element->busy)
				{
					UTIL_LABEL_ENTRY(UTIL_SXOR(L"Shutdown item: ") + element->name);

					if (element->action)
						element->action();
					
					delete element;

					UTIL_LABEL_OK();
					return true;
				}
				else
					return false;
			});
		}

		GetShutdownItemsMutex().unlock();

		UTIL_LABEL_OK();
		Util::Debug::LogSystem::Shutdown();

		#ifdef _DEBUG
		dbg_initialized = false;
		#endif
	});
}

bool Main::IsInShutdown() noexcept { return isInShutdown; }
