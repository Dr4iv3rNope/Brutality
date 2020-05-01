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
#include "../imgui/custom.hpp"
#include "../imgui/imgui_stdlib.h"

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

constexpr int MAX_ITEMS_IN_COLOUMN { 12 };

void Config::DrawMenu()
{
	for (auto& [group, variables] : Config::GetSortedVariables())
	{
		if (ImGui::Begin(group.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Columns(std::max<int>(1U, variables.size() / MAX_ITEMS_IN_COLOUMN));
			auto columns_left = MAX_ITEMS_IN_COLOUMN;

			for (auto& variable : variables)
			{
				switch (variable->GetType())
				{
					case VariableType::Invalid:
						UTIL_DEBUG_ASSERT(false);
						break;

					case VariableType::Unsigned:
					{
						auto var = (LUInt32*)variable;

						auto temp = var->GetValue();

						if (ImGuiCustom::InputAny<std::uint32_t, ImGuiDataType_S32>(
							var->GetKey().c_str(),
							temp,
							variable->IsLimitedVariable() ? std::optional(var->GetMin()) : std::optional<std::uint32_t>(),
							variable->IsLimitedVariable() ? std::optional(var->GetMax()) : std::optional<std::uint32_t>()
							))
							var->SetValue(temp);

						break;
					}

					case VariableType::Signed:
					{
						auto var = (LimitedVariable<int>*)variable;

						int temp = var->GetValue();

						if (ImGuiCustom::InputAny<int, ImGuiDataType_S32>(
							var->GetKey().c_str(),
							temp,
							variable->IsLimitedVariable() ? std::optional(var->GetMin()) : std::optional<int>(),
							variable->IsLimitedVariable() ? std::optional(var->GetMax()) : std::optional<int>()
							))
							var->SetValue(temp);

						break;
					}

					case VariableType::Float:
					{
						auto var = (LimitedVariable<float>*)variable;

						float temp = var->GetValue();

						if (ImGuiCustom::InputAny<float, ImGuiDataType_Float>(
							var->GetKey().c_str(),
							temp,
							variable->IsLimitedVariable() ? std::optional(var->GetMin()) : std::optional<float>(),
							variable->IsLimitedVariable() ? std::optional(var->GetMax()) : std::optional<float>()
						))
							var->SetValue(temp);

						break;
					}

					case VariableType::String:
					{
						auto var = (String<char>*)variable;

						ImGui::InputText
						(
							var->GetKey().data(),
							var->GetBuffer(),
							var->GetMaxLength()
						);

						break;
					}

					case VariableType::Boolean:
					{
						auto var = (Variable<bool>*)variable;

						bool temp = var->GetValue();

						if (ImGui::Checkbox
						(
							var->GetKey().data(),
							&temp
						))
							var->SetValue(temp);

						break;
					}

					case VariableType::Enum:
					{
						auto var = (Enum*)variable;

						int temp = var->GetCurrentItem();

						if (ImGui::Combo
						(
							var->GetKey().data(),
							&temp,
							[] (void* data, int idx, const char** out) -> bool
						{
							auto items = (std::deque<std::string_view>*)data;

							*out = items->at(idx).data();
							return true;
						},
							(void*)&var->GetItems(),
							var->GetItems().size()
							))
							var->SetCurrentItem(temp);

						break;
					}

					case VariableType::Color:
					{
						auto var = (Color*)variable;

						ImVec4 temp = ImGui::ColorConvertU32ToFloat4(var->Hex());

						if (ImGuiCustom::ColorPicker(
							var->GetKey().c_str(),
							temp,
							ImGui::ColorConvertU32ToFloat4(*(std::uint32_t*)&var->GetDefaultColor())
						))
							var->SetValue(ImGui::ColorConvertFloat4ToU32(temp));

						break;
					}

					default:
						UTIL_DEBUG_ASSERT(false);
						break;
				}

				if (!--columns_left)
				{
					ImGui::NextColumn();

					columns_left = MAX_ITEMS_IN_COLOUMN;
				}
			}
		}
		ImGui::End();
	}

	if (ImGui::Begin(UTIL_CXOR("Config"), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static std::string configName;

		enum class ConfigError { None, SuccessLoad, SuccessSave, FailedLoad, FailedSave };
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
				Save(Util::ToWString(configName));
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
				Load(Util::ToWString(configName));
				lastError = ConfigError::SuccessLoad;
			}
			catch (...)
			{
				lastError = ConfigError::FailedLoad;
			}
		}
	}
	ImGui::End();
}
