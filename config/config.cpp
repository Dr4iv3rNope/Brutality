#define NOMINMAX
#include "config.hpp"
#include "variable.hpp"

#include <fstream>
#include <filesystem>
#include <stdexcept>

#include <Windows.h>
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")

#include "../util/strings.hpp"
#include "../util/xorstr.hpp"
#include "../util/debug/logs.hpp"

#include "../nlohmann/json.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"
#include "../imgui/custom/windowmanager.hpp"
#include "../imgui/custom/errors.hpp"

void Config::Save(const std::wstring& filename)
{
	nlohmann::json json_root;

	Config::ExportVariables(json_root);

	{
		std::ofstream file(Config::GetSettingsPath() + filename);

		if (file)
		{
			file << json_root.dump();
			file.close();
		}
		else
			UTIL_XLOG(L"failed to open and save config file");
	}
}

void Config::Load(const std::wstring& filename)
{
	nlohmann::json json_root;

	{
		std::ifstream file(Config::GetSettingsPath() + filename);

		if (file)
		{
			json_root = nlohmann::json::parse(std::string
				{
					std::istream_iterator<char>(file), std::istream_iterator<char>()
				});

			file.close();
		}
		else
			UTIL_XLOG(L"failed to open and load config file");
	}

	Config::ImportVariables(json_root);
}

const std::wstring& Config::GetSettingsPath()
{
	static std::wstring settingsPath;

	if (settingsPath.empty())
	{
		wchar_t* folderPath = new wchar_t[MAX_PATH] {};

		if (SHGetFolderPathW(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, folderPath) != S_OK)
			throw std::runtime_error(UTIL_XOR("failed to get documents folder"));

		settingsPath = std::wstring(folderPath);
		settingsPath += UTIL_SXOR(L"\\Brutality\\");

		if (!std::filesystem::exists(settingsPath))
			if (!std::filesystem::create_directory(settingsPath))
				throw std::runtime_error(UTIL_XOR("failed to create directory"));

		delete[] folderPath;
	}
	
	return settingsPath;
}

static void DrawConfigContent(ImGui::Custom::Window&) noexcept
{
	static std::string configName;

	static constexpr int ERR_NONE { -1 };
	static constexpr int ERR_FAILED_LOAD { 0 };
	static constexpr int ERR_FAILED_SAVE { 1 };

	static int currentError { ERR_NONE };
	static ImGui::Custom::ErrorList errorList =
	{
		ImGui::Custom::Error(
			UTIL_SXOR("Failed to load config"),
			ImVec4(1.f, 0.f, 0.f, 1.f)
		),

		ImGui::Custom::Error(
			UTIL_SXOR("Failed to save config"),
			ImVec4(1.f, 0.f, 0.f, 1.f)
		)
	};

	ImGui::Custom::StatusError(currentError, errorList);

	ImGui::InputText("", &configName);

	if (ImGui::Button(UTIL_CXOR("Save")))
	{
		try
		{
			Config::Save(Util::ToWideChar(configName));
			currentError = ERR_NONE;
		}
		catch (...)
		{
			currentError = ERR_FAILED_SAVE;
		}
	}

	ImGui::SameLine();

	if (ImGui::Button(UTIL_CXOR("Load")))
	{
		try
		{
			Config::Load(Util::ToWideChar(configName));
			currentError = ERR_NONE;
		}
		catch (...)
		{
			currentError = ERR_FAILED_LOAD;
		}
	}
}

void Config::RegisterWindow() noexcept
{
	ImGui::Custom::windowManager.RegisterWindow(
		ImGui::Custom::Window(
			UTIL_SXOR("Config"),
			ImGuiWindowFlags_AlwaysAutoResize,
			DrawConfigContent
		)
	);
}
