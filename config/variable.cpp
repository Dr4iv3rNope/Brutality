#include "variable.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"
#include "../imgui/custom/windowmanager.hpp"
#include "../imgui/custom/numeric.hpp"
#include "../imgui/custom/colors.hpp"
#include "../imgui/custom/variableui.hpp"

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
		if (variable->GetFlags().HasFlag(Config::VariableFlags_NotVisible))
			continue;

		if (variable->GetFlags().HasFlag(Config::VariableFlags_AtNewLine))
			ImGui::NewLine();

		if (variable->GetFlags().HasFlag(Config::VariableFlags_AtSameLine))
			ImGui::SameLine();

		if (variable->GetFlags().HasFlag(Config::VariableFlags_AlignToRight))
			ImGui::SetNextItemWidth(-1.f);

		switch (variable->GetType())
		{
			case Config::VariableType::Invalid:
				UTIL_DEBUG_ASSERT(false);
				break;

			case Config::VariableType::Unsigned:
				if (variable->IsLimitedVariable())
					ImGui::Custom::Variable::LimitedUnsigned(*(Config::LimitedVariable<std::uint32_t>*)variable);
				else
					ImGui::Custom::Variable::Unsigned(*(Config::Variable<std::uint32_t>*)variable);

				break;

			case Config::VariableType::Signed:
				if (variable->IsLimitedVariable())
					ImGui::Custom::Variable::LimitedSigned(*(Config::LimitedVariable<std::int32_t>*)variable);
				else
					ImGui::Custom::Variable::Signed(*(Config::Variable<std::int32_t>*)variable);

				break;

			case Config::VariableType::Float:
				if (variable->IsLimitedVariable())
					ImGui::Custom::Variable::LimitedFloat(*(Config::LimitedVariable<float>*)variable);
				else
					ImGui::Custom::Variable::Float(*(Config::Variable<float>*)variable);

				break;

			case Config::VariableType::String:
				if (variable->IsLimitedVariable())
					ImGui::Custom::Variable::LimitedString(*(Config::LimitedString<char>*)variable);
				else
					ImGui::Custom::Variable::String(*(Config::String<char>*)variable);

				break;

			case Config::VariableType::Boolean:
				ImGui::Custom::Variable::Boolean(*(Config::Bool*)variable);
				break;

			case Config::VariableType::Enum:
				ImGui::Custom::Variable::Enum(*(Config::Enum*)variable);
				break;

			case Config::VariableType::Color:
				ImGui::Custom::Variable::Color(*(Config::Color*)variable);
				break;

			case Config::VariableType::Key:
				ImGui::Custom::Variable::Key(*(Config::Key*)variable);
				break;

			case Config::VariableType::Flags:
				ImGui::Custom::Variable::Flags(*(Config::Flags*)variable);
				break;

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
			return !var->GetFlags().HasFlag(Config::VariableFlags_NotVisible);
		}) == variables.end())
			continue;

		ImGui::Custom::Window window(group, ImGuiWindowFlags_AlwaysAutoResize, DrawWindow);
		window.SetUserData(&variables);

		ImGui::Custom::windowManager->RegisterWindow(window);
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

bool Config::UnregisterVariable(IVariable& variable) noexcept
{
	if (auto iter = GetSortedVariables().find(variable.GetKey());
		iter != GetSortedVariables().end())
	{
		GetSortedVariables().erase(iter);
		return true;
	}
	else
		return false;
}

bool Config::IsVariableRegistered(const std::string& group, const std::string& key) noexcept
{
	if (const auto& group_iter = GetSortedVariables().find(group.data());
		group_iter != GetSortedVariables().end())
		return std::find_if(group_iter->second.begin(), group_iter->second.end(), [key] (IVariable*& var)
		{
			return var->GetKey() == key;
		}) != group_iter->second.end();

	return false;
}

void Config::ImportVariables(const Json::Value& root)
{
	for (auto& [group, variables] : GetSortedVariables())
		if (auto json_group = root[group]; root.isObject())
			for (auto& variable : variables)
				if (const auto& json_value = json_group[variable->GetKey()]; !json_value.isNull())
					if (!variable->Import(json_value))
						UTIL_LOG(UTIL_WFORMAT(
							UTIL_XOR(L"Failed to import variable ") <<
							Util::ToWideChar(variable->GetGroup().data()) << ' ' <<
							Util::ToWideChar(variable->GetKey().data())
						));
}

void Config::ExportVariables(Json::Value& root)
{
	for (const auto& [group, variables] : GetSortedVariables())
	{
		auto& json_group = root[group];

		for (const auto& variable : variables)
		{
			if (variable->GetFlags().HasFlag(Config::VariableFlags_DontSave))
				continue;

			Json::Value temp_json_value;

			if (variable->Export(temp_json_value))
				json_group[variable->GetKey()] = temp_json_value;
			else
				UTIL_LOG(UTIL_WFORMAT(
					UTIL_XOR(L"Failed to export variable ") <<
					Util::ToWideChar(variable->GetGroup().data()) << ' ' <<
					Util::ToWideChar(variable->GetKey().data())
				));
		}
	}
}

void Config::ResetAllVariables() noexcept
{
	for (const auto& [group, variables] : GetSortedVariables())
		for (const auto variable : variables)
			variable->Reset();
}
