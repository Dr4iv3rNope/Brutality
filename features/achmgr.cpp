#include "achmgr.hpp"

#include "../imgui/imgui.h"
#include "../imgui/custom/windowmanager.hpp"
#include "../imgui/custom/errors.hpp"

#include "../util/strings.hpp"
#include "../util/xorstr.hpp"
#include "../util/debug/logs.hpp"
#include "../util/debug/assert.hpp"
#include "../util/debug/labels.hpp"

#include "../sourcesdk/achievementmgr.hpp"
#include "../sourcesdk/engineclient.hpp"

#include "../main.hpp"

#include <map>
#include <string>

enum Err_
{
	Err_None = -1,
	Err_FailedGetMgr,
	Err_FailedGetAch
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
};

static std::map<std::string, SourceSDK::Achievement*> achievementList;
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
				achievementList.insert(std::make_pair(ach->name, ach));
			else
			{
				currentError = Err_FailedGetAch;

				UTIL_LOG(UTIL_WFORMAT(
					UTIL_XOR("Failed to get achievement by index ") << i
				));
			}
		}

		if (currentError != Err_FailedGetAch)
			currentError = Err_None;

		UTIL_LABEL_OK();
	}
	else
	{
		currentError = Err_FailedGetMgr;

		UTIL_XLOG(L"Failed to get achievement manger!");
		UTIL_LABEL_OVERRIDE();
	}
}

static void UnlockAll() noexcept
{
	for (auto& ach : achievementList)
		ach.second->achieved = true;
}

static void LockAll() noexcept
{
	for (auto& ach : achievementList)
		ach.second->achieved = false;
}

static bool UnlockCurrent() noexcept
{
	if (currentAchievement == -1)
		return false;

	UTIL_DEBUG_ASSERT(currentAchievementIter == achievementList.end());

	currentAchievementIter->second->achieved = true;
	return true;
}

static bool LockCurrent() noexcept
{
	if (currentAchievement == -1)
		return false;

	UTIL_DEBUG_ASSERT(currentAchievementIter == achievementList.end());

	currentAchievementIter->second->achieved = false;
	return true;
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

		*out = iter->first.c_str();
		return true;
	}, nullptr, achievementList.size(), 10))
	{
		currentAchievementIter = achievementList.begin();
		std::advance(currentAchievementIter, currentAchievement);
	}

	if (currentAchievement != -1)
	{
		UTIL_DEBUG_ASSERT(currentAchievementIter == achievementList.end());

		ImGui::Text(UTIL_CXOR("Current Achievement: (%i) %s"),
					currentAchievement,
					currentAchievementIter->first.c_str()
		);

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
