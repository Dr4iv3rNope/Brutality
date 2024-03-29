#include "lualoader.hpp"

#if BUILD_GAME_IS_GMOD
#include "../luasdk/luainterface.hpp"

#include "../../imgui/imgui.h"
#include "../../imgui/imgui_stdlib.h"
#include "../../imgui/custom/windowmanager.hpp"
#include "../../imgui/custom/errors.hpp"

#include "../../main.hpp"

#include "../../util/xorstr.hpp"
#include "../../util/strings.hpp"

#include "../../util/debug/logs.hpp"
#include "../../util/debug/labels.hpp"

#include <deque>
#include <fstream>
#include <filesystem>

static std::deque<std::string> luaList;


constexpr int ERR_NOERR { -1 };
constexpr int ERR_FAILED_LOAD { 0 };

static int currentError { ERR_NOERR };
static ImGui::Custom::ErrorList errorList =
{
	ImGui::Custom::Error(
		UTIL_SXOR("Failed to load file"),
		ImVec4(1.f, 0.f, 0.f, 1.f)
	),
};

static inline const std::wstring& GetLuaPath() noexcept
{
	static std::wstring luaPath { Main::GetLocalPath() + UTIL_SXOR(L"lua_scripts\\") };

	return luaPath;
}

static void UpdateLuaList() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Updating lua script list"));

	if (!std::filesystem::is_directory(GetLuaPath()))
	{
		UTIL_XLOG(L"Lua script folder not exist, creating it");

		std::filesystem::create_directory(GetLuaPath());
	}

	luaList.clear();

	for (auto& file : std::filesystem::directory_iterator(GetLuaPath()))
		if (file.is_regular_file() && file.path().has_filename())
			luaList.push_back(file.path().filename().string());

	UTIL_LABEL_OK();
}

static bool GetLuaScript(const std::string& filename, std::string& out) noexcept
{
	const auto path = GetLuaPath() + Util::ToWideChar(filename);
	std::ifstream file(path);

	if (file)
	{
		out = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

		file.close();
		return true;
	}
	else
	{
		UTIL_XLOG(L"Failed to open the lua script!");

		return false;
	}
}

static void DrawMenu(ImGui::Custom::Window&) noexcept
{
	static std::string luaId {};

	ImGui::TextUnformatted(UTIL_CXOR("Identificator"));
	ImGui::PushItemWidth(-1.f);
	ImGui::InputText("", &luaId);

	if (ImGui::BeginTabBar(UTIL_CXOR("#TAB_BAR")))
	{
		if (ImGui::BeginTabItem(UTIL_CXOR("Lua Editor")))
		{
			static std::string luaBuffer {};

			ImGui::PushID(1);
			ImGui::InputTextMultiline("", &luaBuffer, ImVec2(-1.f, 200), ImGuiInputTextFlags_AllowTabInput);
			ImGui::PopID();

			if (*interfaces->luainterface)
			{
				if (ImGui::Button(UTIL_CXOR("Run code")))
					(*interfaces->luainterface)->RunString(luaId.c_str(), luaBuffer.c_str());
			}
			else
				ImGui::TextUnformatted(UTIL_CXOR("Lua Interface not available"));

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem(UTIL_CXOR("Lua List")))
		{
			static auto currentLuaScript { -1 };

			if (ImGui::Button(UTIL_CXOR("Refresh")))
			{
				currentLuaScript = -1;
				UpdateLuaList();
			}

			if (ImGui::IsItemHovered())
			{
				static auto path { Util::ToMultiByte(GetLuaPath()) };

				ImGui::BeginTooltip();
				ImGui::Text(UTIL_CXOR("Lua script path: %s"), path.c_str());
				ImGui::EndTooltip();
			}

			ImGui::PushItemWidth(-1.f);
			ImGui::ListBox("", &currentLuaScript, [] (void*, int idx, const char** out) -> bool
			{
				*out = luaList[idx].c_str();
				return true;
			}, nullptr, luaList.size(), 6);

			if (*interfaces->luainterface)
			{
				ImGui::Custom::StatusError(currentError, errorList);

				if (ImGui::Button(UTIL_CXOR("Run file")))
					if (std::string content; GetLuaScript(luaList[currentLuaScript], content))
						(*interfaces->luainterface)->RunString(luaId.c_str(), content.c_str());
			}
			else
				ImGui::TextUnformatted(UTIL_CXOR("Lua Interface not available"));

			ImGui::EndTabItem();
		}
	}

	ImGui::EndTabBar();
}

void GarrysMod::Features::LuaLoader::RegisterWindow() noexcept
{
	ImGui::Custom::windowManager->RegisterWindow(
		ImGui::Custom::Window(
			UTIL_SXOR("Lua Loader"),
			0,
			DrawMenu
		)
	);
}
#endif
