#include "shutdown.hpp"

#include "imgui/imgui.h"

#include "util/debug/logs.hpp"
#include "util/debug/assert.hpp"
#include "util/debug/labels.hpp"

#include "features/gmod/antiscreengrab.hpp"
#include "features/gmod/luainterface.hpp"
#include "features/namechanger.hpp"

#include <thread>
#include <list>
#include <stdexcept>
#include <filesystem>
#include <mutex>

#include <Windows.h>

static std::atomic_bool isInShutdown { false };

inline auto& GetShutdownItemsMutex() noexcept
{
	static std::mutex shutdownItemsMutex {};

	return shutdownItemsMutex;
}

inline auto& GetShutdownItems() noexcept
{
	static std::list<Main::ShutdownElement*> shutdownItems {};

	return shutdownItems;
}

void Main::AddToShutdown(ShutdownElement* element) noexcept
{
	UTIL_DEBUG_ASSERT(element);
	UTIL_DEBUG_ASSERT(element->action);

	GetShutdownItemsMutex().lock();

	UTIL_DEBUG_ASSERT(
		std::find(GetShutdownItems().begin(), GetShutdownItems().end(), element)
		==
		GetShutdownItems().end()
	);

	GetShutdownItems().push_back(element);
	GetShutdownItemsMutex().unlock();
}

void Main::Shutdown() noexcept
{
	UTIL_DEBUG_ASSERT(!isInShutdown);

	isInShutdown = true;

	static std::thread shutdownThread([] ()
	{
		UTIL_LABEL_ENTRY(UTIL_XOR(L"Shutdown"));
		
		GetShutdownItemsMutex().lock();
		
		while (!GetShutdownItems().empty())
		{
			std::remove_if(GetShutdownItems().begin(), GetShutdownItems().end(), [] (ShutdownElement* element)
			{
				if (!element->busy)
				{
					UTIL_LABEL_ENTRY(UTIL_SXOR(L"Shutdown item: ") + element->name);

					element->action();
					delete element;

					UTIL_LABEL_OK();
					return true;
				}
				else
					return false;
			});
		}

		GetShutdownItemsMutex().unlock();

		UTIL_LABEL_OK();
		Util::Debug::LogSystem::Shutdown();
	});
}

bool Main::IsInShutdown() noexcept { return isInShutdown; }

void Main::DrawMenu() noexcept
{
	if (ImGui::Begin(UTIL_CXOR("Main"), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (ImGui::Button(UTIL_CXOR("Shutdown / Unload hack")))
			Shutdown();
	}
	ImGui::End();
}

Main::ShutdownGuard::ShutdownGuard(ShutdownElement*& element) noexcept
{
	UTIL_DEBUG_ASSERT(element);

	_element = element;
}
