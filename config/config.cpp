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
#include "../util/debug/errors.hpp"

#include "../jsoncpp/value.h"
#include "../jsoncpp/writer.h"
#include "../jsoncpp/reader.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"
#include "../imgui/custom/windowmanager.hpp"
#include "../imgui/custom/errors.hpp"

void Config::Save(const std::wstring& filename)
{
	Json::Value json_root;

	Config::ExportVariables(json_root);

	{
		std::ofstream file(Config::GetSettingsPath() + filename);

		if (file)
		{
			file << json_root;
			file.close();
		}
		else
			UTIL_RUNTIME_ERROR("failed to open and save config file");
	}
}

void Config::Load(const std::wstring& filename)
{
	Json::Value json_root;

	{
		std::ifstream file(Config::GetSettingsPath() + filename);

		if (file)
		{
			file >> json_root;
			file.close();
		}
		else
			UTIL_RUNTIME_ERROR("failed to open and load config file");
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

	enum Err_
	{
		Err_None = -1,
		Err_FailedLoad,
		Err_FailedSave
	};

	static Err_ currentError { Err_None };
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

	ImGui::Custom::StatusError((int&)currentError, errorList);

	ImGui::PushItemWidth(-1.f);
	ImGui::InputText("", &configName);

	if (ImGui::Button(UTIL_CXOR("Save")))
	{
		try
		{
			Config::Save(Util::ToWideChar(configName));
			currentError = Err_None;
		}
		catch (...)
		{
			currentError = Err_FailedSave;
		}
	}

	ImGui::SameLine();

	if (ImGui::Button(UTIL_CXOR("Load")))
	{
		try
		{
			Config::Load(Util::ToWideChar(configName));
			currentError = Err_None;
		}
		catch (...)
		{
			currentError = Err_FailedLoad;
		}
	}

	ImGui::SameLine();

	if (ImGui::Button(UTIL_CXOR("Reset")))
	{
		Config::ResetAllVariables();
	}
}

void Config::RegisterWindow() noexcept
{
	ImGui::Custom::windowManager->RegisterWindow(
		ImGui::Custom::Window(
			UTIL_SXOR("Config"),
			ImGuiWindowFlags_AlwaysAutoResize,
			DrawConfigContent
		)
	);
}
