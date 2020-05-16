#include "achmgr.hpp"

#include "../imgui/imgui.h"
#include "../imgui/custom/windowmanager.hpp"
#include "../imgui/custom/errors.hpp"
#include "../imgui/custom/text.hpp"

#include "../util/strings.hpp"
#include "../util/xorstr.hpp"
#include "../util/debug/logs.hpp"
#include "../util/debug/assert.hpp"
#include "../util/debug/labels.hpp"

#include "../sourcesdk/achievementmgr.hpp"
#include "../sourcesdk/engineclient.hpp"
#include "../sourcesdk/localize.hpp"

#include "../steamapi/steaminterfaces.hpp"

#include "../main.hpp"

#include <deque>
#include <string>

enum Err_
{
	Err_None = -1,
	Err_FailedGetMgr,
	Err_FailedGetAch,
	Err_FailedGetLocal,

	// steam errors

	Err_FailedResetAch,
	Err_FailedStoreStats,
};

static Err_ currentError { Err_None };
static ImGui::Custom::ErrorList errorList
{
	ImGui::Custom::Error(
		UTIL_SXOR("Failed to get achievement manger"),
		ImVec4(1.f, 0.f, 0.f, 1.f)
	),

	ImGui::Custom::Error(
		UTIL_SXOR("Failed to get some achievements"),
		ImVec4(1.f, 1.f, 0.f, 1.f),
		UTIL_SXOR("Check logs for more information")
	),

	ImGui::Custom::Error(
		UTIL_SXOR("Failed to get some localized achievement name/description"),
		ImVec4(1.f, 1.f, 0.f, 1.f),
		UTIL_SXOR("Check logs for more information")
	),

	ImGui::Custom::Error(
		UTIL_SXOR("Failed to reset achievement"),
		ImVec4(1.f, 0.f, 0.f, 1.f)
	),

	ImGui::Custom::Error(
		UTIL_SXOR("Failed to store stats"),
		ImVec4(1.f, 0.f, 0.f, 1.f)
	),
};

struct AchievementInfo
{
private:
	inline void SetupName() noexcept
	{
		const auto name_local_format = UTIL_FORMAT(
			'#' << achievement->name << UTIL_SXOR("_NAME")
		);

		const auto name_local = interfaces->localize->Find(name_local_format.c_str());

		if (name_local)
			name = Util::ToMultiByte(name_local);
		else
		{
			UTIL_LOG(UTIL_WFORMAT(
				UTIL_XOR("Failed to get localized achievement name. ID: ") <<
				achievement->achievementID <<
				UTIL_XOR(". Name: ") <<
				achievement->name
			));

			currentError = Err_FailedGetLocal;

			name = achievement->name;
		}
	}

	inline void SetupDesc() noexcept
	{
		const auto desc_local_format = UTIL_FORMAT(
			'#' << achievement->name << UTIL_SXOR("_DESC")
		);

		const auto desc_local = interfaces->localize->Find(desc_local_format.c_str());

		if (desc_local)
			desc = Util::ToMultiByte(desc_local);
		else
		{
			UTIL_LOG(UTIL_WFORMAT(
				UTIL_XOR("Failed to get localized achievement desctiption. ID: ") <<
				achievement->achievementID <<
				UTIL_XOR(". Name: ") <<
				achievement->name
			));

			currentError = Err_FailedGetLocal;
		}
	}

public:
	std::string name, desc;
	SourceSDK::Achievement* achievement;

	inline AchievementInfo(SourceSDK::Achievement* achievement) noexcept
		: achievement { achievement }
	{
		UTIL_DEBUG_ASSERT(achievement);

		SetupName();
		SetupDesc();
	}

	inline auto* operator->() noexcept { return achievement; }
};

static std::deque<AchievementInfo> achievementList;
static int currentAchievement = -1;
static auto currentAchievementIter = achievementList.end();

static void UpdateAchievementList() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Updating achievement list"));

	if (auto ach_mgr = interfaces->engine->GetAchievementMgr())
	{
		achievementList.clear();
		currentAchievement = -1;
		currentAchievementIter = achievementList.end();

		for (auto i = 0; i < ach_mgr->GetAchievementCount(); i++)
		{
			if (auto ach = ach_mgr->GetAchievementByIndex(i))
				achievementList.push_back(AchievementInfo(ach));
			else
			{
				currentError = Err_FailedGetAch;

				UTIL_LOG(UTIL_WFORMAT(
					UTIL_XOR("Failed to get achievement at index ") << i
				));
			}
		}

		if (currentError != Err_FailedGetAch &&
			currentError != Err_FailedGetLocal)
			currentError = Err_None;

		UTIL_LABEL_OK();
	}
	else
	{
		currentError = Err_FailedGetMgr;

		UTIL_XLOG(L"Failed to get achievement manager!");
		UTIL_LABEL_OVERRIDE();
	}
}

static void LockAll() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Locking all achievements"));

	for (auto& info : achievementList)
	{
		if (!steamInterfaces->userstats->ClearAchievement(info->name))
		{
			currentError = Err_FailedResetAch;

			UTIL_LOG(UTIL_WFORMAT(
				UTIL_XOR(L"Failed to clear achievement ") << info->name
			));
			continue;
		}

		if (!steamInterfaces->userstats->StoreStats())
		{
			currentError = Err_FailedStoreStats;

			UTIL_LOG(UTIL_WFORMAT(
				UTIL_XOR(L"Failed to store stats at achievement ") << info->name
			));
			continue;
		}
	}

	if (currentError != Err_FailedStoreStats &&
		currentError != Err_FailedResetAch)
		currentError = Err_None;

	UTIL_LABEL_OK();
}

static void UnlockAll() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Unlocking all achievements"));

	if (auto ach_mgr = interfaces->engine->GetAchievementMgr())
	{
		LockAll();

		for (auto& info : achievementList)
			ach_mgr->AwardAchievement(info->achievementID);

		if (currentError != Err_FailedStoreStats &&
			currentError != Err_FailedResetAch)
			currentError = Err_None;

		UTIL_LABEL_OK();
	}
	else
	{
		currentError = Err_FailedGetMgr;

		UTIL_XLOG(L"Failed to get achievement manager!");
		UTIL_LABEL_OVERRIDE();
	}
}

static bool LockCurrent() noexcept
{
	if (currentAchievement == -1)
		return false;

	UTIL_LABEL_ENTRY(UTIL_XOR(L"Locking achievement"));
	UTIL_DEBUG_ASSERT(currentAchievementIter != achievementList.end());

	if (!steamInterfaces->userstats->ClearAchievement(
		steamInterfaces->userstats->GetAchievementName(currentAchievement)))
	{
		currentError = Err_FailedResetAch;

		UTIL_LOG(UTIL_WFORMAT(
			UTIL_XOR(L"Failed to clear achievement ") <<
			currentAchievementIter->achievement->name
		));

		UTIL_LABEL_OVERRIDE();
		return false;
	}

	if (!steamInterfaces->userstats->StoreStats())
	{
		currentError = Err_FailedStoreStats;

		UTIL_LOG(UTIL_WFORMAT(
			UTIL_XOR(L"Failed to store stats at achievement ") <<
			currentAchievementIter->achievement->name
		));

		UTIL_LABEL_OVERRIDE();
		return false;
	}

	UTIL_LABEL_OK();
	return true;
}

static bool UnlockCurrent() noexcept
{
	if (currentAchievement == -1)
		return false;

	UTIL_LABEL_ENTRY(UTIL_XOR(L"Unlocking achievement"));
	UTIL_DEBUG_ASSERT(currentAchievementIter != achievementList.end());

	if (auto ach_mgr = interfaces->engine->GetAchievementMgr())
	{
		if (!LockCurrent())
		{
			UTIL_LABEL_OVERRIDE();
			return false;
		}

		ach_mgr->AwardAchievement(currentAchievement);

		currentError = Err_None;
		
		UTIL_LABEL_OK();
		return true;
	}
	else
	{
		currentError = Err_FailedGetMgr;

		UTIL_XLOG(L"Failed to get achievement manager!");

		UTIL_LABEL_OVERRIDE();
		return false;
	}
}

static void DrawWindow(ImGui::Custom::Window&) noexcept
{
	ImGui::Custom::StatusError((int&)currentError, errorList);

	if (ImGui::Button(UTIL_CXOR("Update")))
		UpdateAchievementList();

	ImGui::SameLine();

	if (ImGui::Button(UTIL_CXOR("Unlock all")))
		UnlockAll();

	ImGui::SameLine();

	if (ImGui::Button(UTIL_CXOR("Lock all")))
		LockAll();

	ImGui::PushItemWidth(-1.f);
	if (ImGui::ListBox("", &currentAchievement, [] (void*, int idx, const char** out) -> bool
	{
		auto iter = achievementList.begin();
		std::advance(iter, idx);

		*out = iter->name.c_str();
		return true;
	}, nullptr, achievementList.size(), 10))
	{
		currentAchievementIter = achievementList.begin();
		std::advance(currentAchievementIter, currentAchievement);

		UTIL_LOG(UTIL_WFORMAT(
			UTIL_XOR(L"Selected achievment ") <<
			Util::ToWideChar(currentAchievementIter->name)
		));
	}

	if (currentAchievement != -1)
	{
		UTIL_DEBUG_ASSERT(currentAchievementIter != achievementList.end());

		ImGui::Custom::CopiableText(UTIL_CXOR("Current Achievement: %i"), currentAchievement);
		ImGui::Custom::CopiableText(UTIL_CXOR("Name: %s"), currentAchievementIter->name.c_str());
		ImGui::Custom::CopiableText(UTIL_CXOR("Description: %s"), currentAchievementIter->desc.c_str());

		ImGui::Custom::CopiableText(
			UTIL_CXOR("Is Achieved?: %s"),
			ImGui::Custom::ToTextBoolean(currentAchievementIter->achievement->achieved).c_str()
		);

		ImGui::Custom::CopiableText(
			UTIL_CXOR("Hidden?: %s"),
			ImGui::Custom::ToTextBoolean(currentAchievementIter->achievement->hideUntilAchieved).c_str()
		);

		ImGui::NewLine();

		if (ImGui::Button(UTIL_CXOR("Unlock")))
			UnlockCurrent();

		ImGui::SameLine();
		if (ImGui::Button(UTIL_CXOR("Lock")))
			LockCurrent();
	}
}

void Features::AchievementMgr::RegisterWindow() noexcept
{
	ImGui::Custom::windowManager->RegisterWindow(
		ImGui::Custom::Window(
			UTIL_SXOR("Achievement Manager"),
			ImGuiWindowFlags_AlwaysAutoResize,
			DrawWindow
		)
	);
}
