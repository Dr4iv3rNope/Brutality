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
#include "features/entlist.hpp"
#include "features/chams.hpp"
#include "features/chatspam.hpp"
#include "features/achmgr.hpp"
#include "gmod/features/lualoader.hpp"
#include "gmod/features/antiscreengrab.hpp"

#include "gmod/luasdk/luainterface.hpp"

#include "config/config.hpp"
#include "config/variable.hpp"

#include "ui/ui.hpp"


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

static Config::Bool v_bool("Debug", "Boolean", true, Config::VariableFlags_DontSave);
static Config::Int32 v_int32("Debug", "Integer 32", Config::VariableFlags_DontSave);
static Config::UInt32 v_uint32("Debug", "Unsigned Integer 32", Config::VariableFlags_DontSave);
static Config::Float v_float("Debug", "Float", Config::VariableFlags_DontSave);

static Config::LInt32 lv_int32("Debug", "Limited Integer 32", 0, -228, 1337, Config::VariableFlags_DontSave);
static Config::LUInt32 lv_uint32("Debug", "Limited Unsigned Integer 32", 666, 666, 1234, Config::VariableFlags_DontSave);
static Config::LFloat lv_float("Debug", "Limited Float", 0, -3.14f, 2.28f, Config::VariableFlags_DontSave);
static Config::LimitedString<char> lv_string("Debug", "Your Name", 64, "AYE", Config::VariableFlags_DontSave);

static Config::String<char> uv_string("Debug", "XYZ", "123", Config::VariableFlags_DontSave);
static Config::Enum uv_enum("Debug", "Who are you", { "Idiot", "I Have Stupid", "Yes sir" }, Config::VariableFlags_DontSave);
static Config::Color uv_color("Debug", "Pick some color", { 1, 3, 3, 7 }, Config::VariableFlags_DontSave);
static Config::Key uv_key("Debug", "Some Key Binding", ImGui::Custom::Key::_Invalid, Config::VariableFlags_DontSave);
static Config::Flags uv_flags("Debug", "Flags", { "flag 1", "flag 2", "pride flag", "flag 3" }, Config::VariableFlags_DontSave);
#endif

Config::Color playerColorNormal(UTIL_SXOR("Player Colors"), UTIL_SXOR("Normals Color"), { 255, 255, 255, 255 });
Config::Color playerColorDangerous(UTIL_SXOR("Player Colors"), UTIL_SXOR("Dangerouses Color"), { 255, 0, 0, 155 });
Config::Color playerColorFriends(UTIL_SXOR("Player Colors"), UTIL_SXOR("Friends Color"), { 55, 255, 55, 255 });
Config::Color playerColorRages(UTIL_SXOR("Player Colors"), UTIL_SXOR("Rages Color"), { 255, 100, 0, 255 });


// esp

Config::Bool espEnabled(UTIL_SXOR("ESP"), UTIL_SXOR("Enable"));
Config::LFloat espMaxDistance(UTIL_SXOR("ESP"), UTIL_SXOR("Max Distance"), 0.f, 0.f, 50000.f);

Config::Bool espUpdatePerFrame(UTIL_SXOR("ESP"), UTIL_SXOR("Update Per Frame (can reduce fps)"), false, Config::VariableFlags_AtNewLine);

Config::Bool espDrawEntities(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Entities"), false, Config::VariableFlags_AtNewLine);
Config::Bool espDrawNormal(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Normal"));
Config::Bool espDrawDangerous(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Dangerous"));
Config::Bool espDrawFriends(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Friends"));
Config::Bool espDrawRages(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Rages"));
Config::Bool espDrawDeadPlayers(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Dead Players"));
Config::Bool espDrawDormantPlayers(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Dormant Players"));

Config::Bool espHealth(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Health"), false, Config::VariableFlags_AtNewLine);
Config::Color espHealthColor(UTIL_SXOR("ESP"), UTIL_SXOR("Health Text Color"), { 55, 255, 55, 255 });

Config::Bool espName(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Name"), false, Config::VariableFlags_AtNewLine);
Config::Color espNameColor(UTIL_SXOR("ESP"), UTIL_SXOR("Name Text Color"), { 55, 255, 255, 255 });

Config::Bool espSkeleton(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Skeleton"), false, Config::VariableFlags_AtNewLine);
#ifdef _DEBUG
Config::Bool dbg_espSkeleton(UTIL_SXOR("ESP"), UTIL_SXOR("Debug Skeleton"));
#endif
Config::LFloat espSkeletonThickness(UTIL_SXOR("ESP"), UTIL_SXOR("Skeleton Thickness"), 1.f, 0.1f, 12.f);

Config::Bool espActiveWeapon(UTIL_SXOR("ESP"), UTIL_SXOR("Draw Active Weapon"), false, Config::VariableFlags_AtNewLine);
Config::Color espActiveWeaponColor(UTIL_SXOR("ESP"), UTIL_SXOR("Active Weapon Text Color"), { 255, 150, 55, 255 });


// text radar

Config::Bool textRadarEnable(UTIL_SXOR("Text Radar"), UTIL_SXOR("Enable"));

Config::UInt32 textRadarX(UTIL_SXOR("Text Radar"), UTIL_SXOR("X"), 0);
Config::UInt32 textRadarY(UTIL_SXOR("Text Radar"), UTIL_SXOR("Y"), 0);

Config::LUInt32 textRadarMaxPlayers(UTIL_SXOR("Text Radar"), UTIL_SXOR("Max Players"), 10, 5, 32);
Config::LFloat textRadarMaxDistance(UTIL_SXOR("Text Radar"), UTIL_SXOR("Max Distance"), 0.f, 0.f, 50000.f);

Config::Bool textRadarDrawDistance(UTIL_SXOR("Text Radar"), UTIL_SXOR("Draw Distance"));


// spectator list
Config::Bool spectatorListEnable(UTIL_SXOR("Spectator List"), UTIL_SXOR("Enable"));

Config::UInt32 spectatorListX(UTIL_SXOR("Spectator List"), UTIL_SXOR("X"));
Config::UInt32 spectatorListY(UTIL_SXOR("Spectator List"), UTIL_SXOR("Y"));

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
Config::LUInt32 bhopSkipTicks(UTIL_SXOR("Bunny Hop"), UTIL_SXOR("Skip ticks"), 0, 0, 100);


// chat spam

Config::Bool chatSpamEnable(
	UTIL_SXOR("Chat Spammer"),
	UTIL_SXOR("Enable"),
	false,
	Config::VariableFlags_NotVisible
);

Config::Bool chatSpamTeam(
	UTIL_SXOR("Chat Spammer"),
	UTIL_SXOR("Team chat mode"),
	false,
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
	UTIL_SXOR("\\n\\nBrutality B-)\\n\\n"),
	Config::VariableFlags_NotVisible
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

Config::Bool uiUseTabs(UTIL_SXOR("Interface"), UTIL_SXOR("Use Tabs instead of windows"), true, Config::VariableFlags_NotVisible);
Config::Enum uiColorStyle(
	UTIL_SXOR("Interface"),
	UTIL_SXOR("UI Color Style"),
	{
		UTIL_SXOR("Default"),
		UTIL_SXOR("Custom All"),
		UTIL_SXOR("Custom Borders")
	},
	Config::VariableFlags_NotVisible
);
Config::Color uiColor(UTIL_SXOR("Interface"), UTIL_SXOR("UI Color"), { 215, 130, 50, 100 }, Config::VariableFlags_NotVisible);
Config::Color uiTextColor(UTIL_SXOR("Interface"), UTIL_SXOR("UI Text Color"), { 255, 255, 255, 255 }, Config::VariableFlags_NotVisible);


// name changer

Config::String<char> nameChangerName(UTIL_SXOR("Name Changer"), UTIL_SXOR("Name"), std::string(), Config::VariableFlags_NotVisible);
Config::Bool nameChangerAutoSend(UTIL_SXOR("Name Changer"), UTIL_SXOR("Auto Send"), false, Config::VariableFlags_NotVisible);


// chams

Config::Bool chamsEnable(UTIL_SXOR("Chams"), UTIL_SXOR("Enable"));
Config::Bool chamsDrawDormant(UTIL_SXOR("Chams"), UTIL_SXOR("Draw Dormant"));

std::initializer_list chamsTypes =
{
	UTIL_SXOR("Disable"),
	UTIL_SXOR("Normal"),
	UTIL_SXOR("Flat"),
	UTIL_SXOR("Shiny"),
	UTIL_SXOR("Glow"),
	UTIL_SXOR("Animated Spawn Effect")
};

Config::Color chamsNormalsVisÑolor(
	UTIL_SXOR("Chams"),
	UTIL_SXOR("Visible Normal Players"),
	{ 155, 155, 155, 255 },
	Config::VariableFlags_AtNewLine
);
Config::Enum chamsNormalsVisType(
	UTIL_SXOR("Chams"),
	UTIL_SXOR("Visible Normal Players Type"),
	chamsTypes,
	Config::VariableFlags_AtSameLine | Config::VariableFlags_AlignToRight
);
Config::Color chamsNormalsOccColor(
	UTIL_SXOR("Chams"),
	UTIL_SXOR("Occluded Normal Players"),
	{ 100, 100, 100, 255 }
);
Config::Enum chamsNormalsOccType(
	UTIL_SXOR("Chams"),
	UTIL_SXOR("Occluded Normal Players Type"),
	chamsTypes,
	Config::VariableFlags_AtSameLine | Config::VariableFlags_AlignToRight
);

Config::Color chamsDangerousVisÑolor(
	UTIL_SXOR("Chams"),
	UTIL_SXOR("Visible Dangerous Players"),
	{ 155, 0, 0, 255 },
	Config::VariableFlags_AtNewLine
);
Config::Enum chamsDangerousVisType(
	UTIL_SXOR("Chams"),
	UTIL_SXOR("Visible Dangerous Players Type"),
	chamsTypes,
	Config::VariableFlags_AtSameLine | Config::VariableFlags_AlignToRight
);
Config::Color chamsDangerousOccColor(
	UTIL_SXOR("Chams"),
	UTIL_SXOR("Occluded Dangerous Players"),
	{ 100, 0, 0, 255 }
);
Config::Enum chamsDangerousOccType(
	UTIL_SXOR("Chams"),
	UTIL_SXOR("Occluded Dangerous Players Type"),
	chamsTypes,
	Config::VariableFlags_AtSameLine | Config::VariableFlags_AlignToRight
);

Config::Color chamsFriendsVisÑolor(
	UTIL_SXOR("Chams"),
	UTIL_SXOR("Visible Friends Players"),
	{ 0, 200, 0, 255 },
	Config::VariableFlags_AtNewLine
);
Config::Enum chamsFriendsVisType(
	UTIL_SXOR("Chams"),
	UTIL_SXOR("Visible Friends Players Type"),
	chamsTypes,
	Config::VariableFlags_AtSameLine | Config::VariableFlags_AlignToRight
);
Config::Color chamsFriendsOccColor(
	UTIL_SXOR("Chams"),
	UTIL_SXOR("Occluded Friends Players"),
	{ 0, 100, 0, 255 }
);
Config::Enum chamsFriendsOccType(
	UTIL_SXOR("Chams"),
	UTIL_SXOR("Occluded Friends Players Type"),
	chamsTypes,
	Config::VariableFlags_AtSameLine | Config::VariableFlags_AlignToRight
);

Config::Color chamsRagesVisÑolor(
	UTIL_SXOR("Chams"), 
	UTIL_SXOR("Visible Rages Players"),
	{ 255, 100, 0, 255 },
	Config::VariableFlags_AtNewLine
);
Config::Enum chamsRagesVisType(
	UTIL_SXOR("Chams"), 
	UTIL_SXOR("Visible Rages Players Type"),
	chamsTypes,
	Config::VariableFlags_AtSameLine | Config::VariableFlags_AlignToRight
);
Config::Color chamsRagesOccColor(
	UTIL_SXOR("Chams"), 
	UTIL_SXOR("Occluded Rages Players"),
	{ 155, 55, 0, 255 }
);
Config::Enum chamsRagesOccType(
	UTIL_SXOR("Chams"), 
	UTIL_SXOR("Occluded Rages Players Type"),
	chamsTypes,
	Config::VariableFlags_AtSameLine | Config::VariableFlags_AlignToRight
);


// trigger bot

Config::Bool triggerbotEnable(UTIL_SXOR("Trigger Bot"), UTIL_SXOR("Enable"));
Config::Bool triggerbotTargetNormals(UTIL_SXOR("Trigger Bot"), UTIL_SXOR("Target At Normal Players"), false, Config::VariableFlags_AtNewLine);
Config::Bool triggerbotTargetDangerous(UTIL_SXOR("Trigger Bot"), UTIL_SXOR("Target At Dangerous Players"), false);
Config::Bool triggerbotTargetFriends(UTIL_SXOR("Trigger Bot"), UTIL_SXOR("Target At Friends Players"), false);
Config::Bool triggerbotTargetRages(UTIL_SXOR("Trigger Bot"), UTIL_SXOR("Target At Rages Players"), false);
Config::Key triggerbotKey(UTIL_SXOR("Trigger Bot"), UTIL_SXOR("hold key"), ImGui::Custom::Key::_Invalid, Config::VariableFlags_AtNewLine);
Config::LFloat triggerbotDelay(UTIL_SXOR("Trigger Bot"), UTIL_SXOR("Delay in seconds"), 0.f, 0.f, 1.f, Config::VariableFlags_AtNewLine);

#if BUILD_GAME_IS_GMOD
// air stuck

Config::Bool airstuckEnable(UTIL_SXOR("Air Stuck"), UTIL_SXOR("Enable"));
Config::Key airstuckKey(UTIL_SXOR("Air Stuck"), UTIL_SXOR("Key"));
Config::LUInt32 airstuckMaxTicks(UTIL_SXOR("Air Stuck"), UTIL_SXOR("Max Ticks"), 400, 50, 1000, Config::VariableFlags_AtNewLine);


// fake duck

Config::Bool fakeduckEnable(UTIL_SXOR("Fake Duck"), UTIL_SXOR("Enable"));
Config::Key fakeduckKey(UTIL_SXOR("Fake Duck"), UTIL_SXOR("Key"), ImGui::Custom::Key::Ctrl);

#endif

// backtrack

Config::Bool backtrackEnable(UTIL_SXOR("Backtrack"), UTIL_SXOR("Enable"));
Config::Bool backtrackValidateOnUpdate(UTIL_SXOR("Backtrack"), UTIL_SXOR("Validate On Update (can create problems)"));
Config::Bool backtrackCalcBones(UTIL_SXOR("Backtrack"), UTIL_SXOR("Calculate backtrack by bones (slower)"));
Config::LFloat backtrackMaxTime(UTIL_SXOR("Backtrack"), UTIL_SXOR("Max Time"), 0.f, 0.f, 0.2f);
Config::LFloat backtrackMaxSimTime(UTIL_SXOR("Backtrack"), UTIL_SXOR("Max Simulation Time"), 1.f, 0.f, 10.0f);
Config::LFloat backtrackMaxFov(UTIL_SXOR("Backtrack"), UTIL_SXOR("Max Fov (can increase fps)"), 0.f, 0.f, 180.f, Config::VariableFlags_AtNewLine);
Config::Bool backtrackValidateFovOnUpdate(UTIL_CXOR("Backtrack"), UTIL_SXOR("Validate Fov On Update"));
Config::Bool backtrackTrackNormals(UTIL_SXOR("Backtrack"), UTIL_SXOR("Track Normal Players"), false, Config::VariableFlags_AtNewLine);
Config::Bool backtrackTrackDangerous(UTIL_SXOR("Backtrack"), UTIL_SXOR("Track Dangerous Players"));
Config::Bool backtrackTrackFriends(UTIL_SXOR("Backtrack"), UTIL_SXOR("Track Friend Players"));
Config::Bool backtrackTrackRages(UTIL_SXOR("Backtrack"), UTIL_SXOR("Track Rage Players"));
Config::Bool backtrackDrawDelay(UTIL_SXOR("Backtrack"), UTIL_SXOR("Draw Delay In Lines"), false, Config::VariableFlags_AtNewLine);
Config::Bool backtrackDrawHeadDots(UTIL_SXOR("Backtrack"), UTIL_SXOR("Draw Head Dots"));

Config::Color backtrackChamsVisColor(
	UTIL_SXOR("Backtrack"),
	UTIL_SXOR("Visible Chams"),
	{ 0, 100, 0, 255 },
	Config::VariableFlags_AtNewLine
);
Config::Enum backtrackChamsVisType(
	UTIL_SXOR("Backtrack"),
	UTIL_SXOR("Visible Chams Color"),
	chamsTypes,
	Config::VariableFlags_AtSameLine | Config::VariableFlags_AlignToRight
);

Config::Color backtrackChamsOccColor(
	UTIL_SXOR("Backtrack"),
	UTIL_SXOR("Occluded Chams"),
	{ 0, 100, 0, 255 }
);
Config::Enum backtrackChamsOccType(
	UTIL_SXOR("Backtrack"),
	UTIL_SXOR("Occluded Chams Color"),
	chamsTypes,
	Config::VariableFlags_AtSameLine | Config::VariableFlags_AlignToRight
);

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
	Features::EntityList::RegisterEntityListWindow();
	Features::PlayerList::RegisterWindow();
	Features::ChatSpam::RegisterWindow();
	Features::CustomDisconnect::RegisterWindow();
	Features::AchievementMgr::RegisterWindow();
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
	::steamInterfaces = new SteamInterfaces();

	InitializeHelpers();
	InitializeFeatures();
	UI::Initialize();

	::hooks = new GameHooks();
	::hooks->Initialize();

	RegisterWindows();

	#ifdef _DEBUG
	dbg_initialized = true;
	#endif
}

static HMODULE thisModule;

BOOL WINAPI DllMain(HMODULE this_module, DWORD attach_reason, LPVOID)
{
	thisModule = this_module;
	DisableThreadLibraryCalls(this_module);

	switch (attach_reason)
	{
		case DLL_PROCESS_ATTACH:
			Main::Initialize();
			break;

		default:
			break;
	}

	return TRUE;
}

#pragma endregion

//
// shutdown
//
static bool isInShutdown { false };

bool Main::IsInShutdown() noexcept
{
	return isInShutdown;
}

void Main::Shutdown() noexcept
{
	UTIL_DEBUG_ASSERT(dbg_initialized);
	UTIL_DEBUG_ASSERT(thisModule);
	UTIL_DEBUG_ASSERT(!isInShutdown);

	isInShutdown = true;

	static std::thread shutdownThread([] ()
	{
		UTIL_LABEL_ENTRY(UTIL_XOR(L"Shutdown"));

		while (Shutdown::Guard::GetActiveGuards() != 0) {}

		UI::Shutdown();

		delete ::hooks;

		Features::Chams::Shutdown();
		#if BUILD_GAME_IS_GMOD
		GarrysMod::Features::AntiScreenGrab::Shutdown();
		GarrysMod::LuaInterface::Shutdown();
		#endif

		delete ::interfaces;
		delete ::steamInterfaces;

		UTIL_LABEL_OK();
		Util::Debug::LogSystem::Shutdown();

		#ifdef _DEBUG
		dbg_initialized = false;
		#endif
		isInShutdown = false;

		FreeLibraryAndExitThread(thisModule, EXIT_SUCCESS);
	});
}
