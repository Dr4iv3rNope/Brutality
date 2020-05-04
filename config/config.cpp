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

	enum class ConfigError {
		None, SuccessLoad, SuccessSave, FailedLoad, FailedSave
	};
	static ConfigError lastError { ConfigError::None };

	switch (lastError)
	{
		//
		// save
		//

		case ConfigError::SuccessSave:
			ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), UTIL_CXOR("Successfully saved config"));
			break;

		case ConfigError::FailedSave:
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), UTIL_CXOR("Failed to save config"));
			break;

		//
		// load
		//

		case ConfigError::SuccessLoad:
			ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), UTIL_CXOR("Successfully loaded config"));
			break;

		case ConfigError::FailedLoad:
			ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), UTIL_CXOR("Failed to load config"));
			break;

		default:
			ImGui::NewLine();
			break;
	}

	ImGui::InputText("", &configName);

	if (ImGui::Button(UTIL_CXOR("Save")))
	{
		try
		{
			Config::Save(Util::ToWString(configName));
			lastError = ConfigError::SuccessSave;
		}
		catch (...)
		{
			lastError = ConfigError::FailedSave;
		}
	}

	ImGui::SameLine();

	if (ImGui::Button(UTIL_CXOR("Load")))
	{
		try
		{
			Config::Load(Util::ToWString(configName));
			lastError = ConfigError::SuccessLoad;
		}
		catch (...)
		{
			lastError = ConfigError::FailedLoad;
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
