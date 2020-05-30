#include "entlist.hpp"

#include "../util/xorstr.hpp"

#include "../main.hpp"

#include "../sourcesdk/clientstate.hpp"
#include "../sourcesdk/globals.hpp"
#include "../sourcesdk/entity.hpp"
#include "../sourcesdk/entitylist.hpp"

#include "../imgui/custom/windowmanager.hpp"
#include "../imgui/custom/colors.hpp"

#include <deque>
#include <mutex>

static inline auto& GetEntityClassnames() noexcept
{
	static std::deque<Features::EntityList::EntitySettings> entityClassnames;

	return entityClassnames;
}

static inline auto& GetEntityClassnamesMutex() noexcept
{
	static std::mutex entityClassnamesMutex;

	return entityClassnamesMutex;
}

const Features::EntityList::EntitySettings* Features::EntityList::GetEntitySettings(const std::string& classname) noexcept
{
	const auto hash = Util::MakeRuntimeHash(classname);

	auto _guard = std::lock_guard(GetEntityClassnamesMutex());
	if (!GetEntityClassnames().empty())
		if (auto iter = std::find_if(GetEntityClassnames().begin(), GetEntityClassnames().end(), [&hash] (const EntitySettings& settings) -> bool
		{
			return settings.classnameHash == hash;
		}); iter != GetEntityClassnames().end())
			return &*iter;

	return nullptr;
}

static void DrawEntityListMenu(ImGui::Custom::Window&) noexcept
{
	if (ImGui::Button(UTIL_CXOR("Dump class names")))
		// make sure we're in game
		if (interfaces->clientstate->IsInGame())
			// setting i to maxClients because
			// we dont need capture C_World and Players
			for (auto i = interfaces->globals->maxClients; i < interfaces->entitylist->GetMaxEntities(); i++)
				if (auto entity = SourceSDK::BaseEntity::GetByIndex(i); entity)
					if (auto classname = entity->GetClassname(); classname &&
						// if entity's class name is not registered then ...
						Features::EntityList::GetEntitySettings(classname) == nullptr)
					{
						GetEntityClassnamesMutex().lock();

						// ... we will register entity's class name
						GetEntityClassnames().push_back(Features::EntityList::EntitySettings(classname));

						GetEntityClassnamesMutex().unlock();
					}

	static int currentItem { -1 };

	ImGui::PushItemWidth(-1.f);
	GetEntityClassnamesMutex().lock();
	ImGui::Combo("", &currentItem, [] (void* data, int idx, const char** out_text) -> bool
	{
		*out_text = GetEntityClassnames()[idx].classname.c_str();
		return true;
	}, nullptr, GetEntityClassnames().size());
	GetEntityClassnamesMutex().unlock();

	if (currentItem != -1)
	{
		static Features::EntityList::EntitySettings* settings;

		GetEntityClassnamesMutex().lock();
		settings = &GetEntityClassnames()[currentItem];
		GetEntityClassnamesMutex().unlock();


		ImGui::Checkbox(UTIL_CXOR("Draw in ESP"), &settings->espDraw);

		if (ImVec4 color = ImGui::ColorConvertU32ToFloat4(settings->espColor);
			ImGui::Custom::ColorPicker(UTIL_CXOR("Draw Color"), color))
			settings->espColor = ImGui::ColorConvertFloat4ToU32(color);

		ImGui::NewLine();

		if (ImVec4 color = ImGui::ColorConvertU32ToFloat4(settings->chamsVisColor);
			ImGui::Custom::ColorPicker(UTIL_CXOR("Chams Visible"), color))
			settings->chamsVisColor = ImGui::ColorConvertFloat4ToU32(color);

		ImGui::SameLine();

		ImGui::PushID(1);
		ImGui::PushItemWidth(-1.f);
		ImGui::Combo(
			"",
			&settings->chamsVisType,
			UTIL_CXOR("Disable\0Normal\0Flat\0Shiny\0Glow\0Animated Spawn Effect\0")
		);
		ImGui::PopID();


		if (ImVec4 color = ImGui::ColorConvertU32ToFloat4(settings->chamsOccColor);
			ImGui::Custom::ColorPicker(UTIL_CXOR("Chams Occluded"), color))
			settings->chamsOccColor = ImGui::ColorConvertFloat4ToU32(color);

		ImGui::SameLine();

		ImGui::PushID(2);
		ImGui::PushItemWidth(-1.f);
		ImGui::Combo(
			"",
			&settings->chamsOccType,
			UTIL_CXOR("Disable\0Normal\0Flat\0Shiny\0Glow\0Animated Spawn Effect\0")
		);
		ImGui::PopID();
	}
	else
		ImGui::TextUnformatted(UTIL_CXOR("Choose item from list above"));
}

void Features::EntityList::RegisterEntityListWindow() noexcept
{
	ImGui::Custom::windowManager->RegisterWindow(
		ImGui::Custom::Window(
			UTIL_SXOR("Entity List"),
			ImGuiWindowFlags_AlwaysAutoResize,
			DrawEntityListMenu
		)
	);
}
