#include "variable.hpp"

#include "../util/strings.hpp"
#include "../util/stringhash.hpp"
#include "../util/debug/labels.hpp"

Config::SortedVariables& Config::GetSortedVariables()
{
	static Config::SortedVariables sortedVariabels;

	return sortedVariabels;
}

bool Config::RegisterVariable(IVariable& variable)
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

bool Config::IsVariableRegistered(std::string_view group, std::string_view key)
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
