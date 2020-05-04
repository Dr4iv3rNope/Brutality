#include "variable.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"
#include "../imgui/custom/windowmanager.hpp"
#include "../imgui/custom/numeric.hpp"
#include "../imgui/custom/colors.hpp"

#include "../util/strings.hpp"
#include "../util/stringhash.hpp"
#include "../util/debug/labels.hpp"

Config::SortedVariables& Config::GetSortedVariables() noexcept
{
	static Config::SortedVariables sortedVariabels;

	return sortedVariabels;
}

static void DrawWindow(ImGui::Custom::Window& window) noexcept
{
	UTIL_DEBUG_ASSERT(window.GetUserData());

	auto& variables = *(Config::Variables*)window.GetUserData();

	for (auto& variable : variables)
	{
		if (!variable->IsVisible())
			continue;

		switch (variable->GetType())
		{
			case Config::VariableType::Invalid:
				UTIL_DEBUG_ASSERT(false);
				break;

			case Config::VariableType::Unsigned:
			{
				auto var = (Config::LimitedVariable<std::uint32_t>*)variable;

				if (auto temp = var->GetValue();
					ImGui::Custom::InputAny<std::uint32_t, ImGuiDataType_U32>(
						var->GetKey().c_str(),
						temp,
						var->IsLimitedVariable() ? std::optional(var->GetMin()) : std::optional<std::uint32_t>(),
						var->IsLimitedVariable() ? std::optional(var->GetMax()) : std::optional<std::uint32_t>()
						))
					var->SetValue(temp);

				break;
			}

			case Config::VariableType::Signed:
			{
				auto var = (Config::LimitedVariable<int>*)variable;

				if (auto temp = var->GetValue();
					ImGui::Custom::InputAny<int, ImGuiDataType_S32>(
						var->GetKey().c_str(),
						temp,
						var->IsLimitedVariable() ? std::optional(var->GetMin()) : std::optional<int>(),
						var->IsLimitedVariable() ? std::optional(var->GetMax()) : std::optional<int>()
						))
					var->SetValue(temp);

				break;
			}

			case Config::VariableType::Float:
			{
				auto var = (Config::LimitedVariable<float>*)variable;

				if (auto temp = var->GetValue();
					ImGui::Custom::InputAny<float, ImGuiDataType_Float>(
						var->GetKey().c_str(),
						temp,
						var->IsLimitedVariable() ? std::optional(var->GetMin()) : std::optional<float>(),
						var->IsLimitedVariable() ? std::optional(var->GetMax()) : std::optional<float>()
						))
					var->SetValue(temp);

				break;
			}

			case Config::VariableType::String:
			{
				if (variable->IsLimitedVariable())
				{
					auto var = (Config::LimitedString<char>*)variable;

					ImGui::InputText(
						var->GetKey().data(),
						var->GetBuffer(),
						var->GetMaxLength()
					);
				}
				else
				{
					auto var = (Config::String<char>*)variable;

					if (std::string temp = var->GetValue();
						ImGui::InputText(var->GetKey().data(), &temp))
						var->SetValue(temp);
				}

				break;
			}

			case Config::VariableType::Boolean:
			{
				auto var = (Config::Variable<bool>*)variable;

				if (bool temp = var->GetValue();
					ImGui::Checkbox(
						var->GetKey().data(),
						&temp
					))
					var->SetValue(temp);

				break;
			}

			case Config::VariableType::Enum:
			{
				auto var = (Config::Enum*)variable;

				static auto callback = [] (void* data, int idx, const char** out) -> bool
				{
					auto items = (std::deque<std::string_view>*)data;

					*out = items->at(idx).data();
					return true;
				};

				if (int temp = var->GetCurrentItem();
					ImGui::Combo(
						var->GetKey().data(),
						&temp,
						callback,
						(void*)&var->GetItems(),
						var->GetItems().size()
					))
					var->SetCurrentItem(temp);
				
				break;
			}

			case Config::VariableType::Color:
			{
				auto var = (Config::Color*)variable;

				if (ImVec4 temp = ImGui::ColorConvertU32ToFloat4(var->Hex());
					ImGui::Custom::ColorPicker(
						var->GetKey().c_str(),
						temp,
						ImGui::ColorConvertU32ToFloat4(*(std::uint32_t*) & var->GetDefaultColor())
					))
					var->SetValue(ImGui::ColorConvertFloat4ToU32(temp));

				break;
			}

			case Config::VariableType::Key:
			{
				auto var = (Config::Key*)variable;

				if (auto temp = var->GetKeyValue();
					ImGui::Custom::InputKey(var->GetKey().c_str(), temp))
					var->SetKeyValue(temp);

				break;
			}

			default:
				UTIL_DEBUG_ASSERT(false);
				break;
		}
	}
}

void Config::RegisterVariablesInWindowManager() noexcept
{
	for (auto& [group, variables] : Config::GetSortedVariables())
	{
		// dont register window if all variables in this group is not visible
		if (std::find_if(variables.begin(), variables.end(), [] (const IVariable* var) -> bool
		{
			return var->IsVisible();
		}) == variables.end())
			continue;

		ImGui::Custom::Window window(group, ImGuiWindowFlags_AlwaysAutoResize, DrawWindow);
		window.SetUserData(&variables);

		ImGui::Custom::windowManager.RegisterWindow(window);
	}
}

bool Config::RegisterVariable(IVariable& variable) noexcept
{
	if (IsVariableRegistered(variable))
	{
		UTIL_DEBUG_ASSERT(false);
		return false;
	}

	// sort variable
	{
		auto group_iter = GetSortedVariables().find(variable.GetGroup().data());

		// if group found
		if (group_iter != GetSortedVariables().end())
			group_iter->second.push_back(&variable);
		else
		{
			// if group not found
			// creating new one

			Variables vars;
			vars.push_back(&variable);

			GetSortedVariables().insert(std::make_pair(variable.GetGroup(), vars));
		}
	}

	return true;
}

bool Config::IsVariableRegistered(std::string_view group, std::string_view key) noexcept
{
	if (const auto& group_iter = GetSortedVariables().find(group.data());
		group_iter != GetSortedVariables().end())
		return std::find_if(group_iter->second.begin(), group_iter->second.end(), [key] (IVariable*& var)
		{
			return var->GetKey() == key;
		}) != group_iter->second.end();

	return false;
}

void Config::ImportVariables(const nlohmann::json& root)
{
	for (auto& [group, variables] : GetSortedVariables())
		if (auto group_hash = UTIL_RUNTIME_STR_HASH(group); root.contains(group_hash))
			if (const auto& json_group = root[group_hash]; json_group.is_object())
				for (auto& variable : variables)
					if (auto key_hash = UTIL_RUNTIME_STR_HASH(variable->GetKey()); json_group.contains(key_hash))
						if (const auto& json_value = json_group[key_hash]; !json_value.is_null())
							if (!variable->Import(json_value))
								UTIL_LOG(UTIL_WFORMAT(
									UTIL_XOR(L"Failed to import variable ") <<
									Util::ToWString(variable->GetGroup().data()) << ' ' <<
									Util::ToWString(variable->GetKey().data())
								));
}

void Config::ExportVariables(nlohmann::json& root)
{
	for (const auto& [group, variables] : GetSortedVariables())
	{
		auto& json_group = root[UTIL_RUNTIME_STR_HASH(group)];

		for (const auto& variable : variables)
		{
			nlohmann::json temp_json_value;

			if (variable->Export(temp_json_value))
				json_group[UTIL_RUNTIME_STR_HASH(variable->GetKey())] = temp_json_value;
			else
				UTIL_LOG(UTIL_WFORMAT(
					UTIL_XOR(L"Failed to export variable ") <<
					Util::ToWString(variable->GetGroup().data()) << ' ' <<
					Util::ToWString(variable->GetKey().data())
				));
		}
	}
}
