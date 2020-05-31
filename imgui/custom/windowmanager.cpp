#include "windowmanager.hpp"
#include "variableui.hpp"

#include "../../main.hpp"

#include "../../config/variable.hpp"

#include "../../util/strings.hpp"

#include "../../util/debug/labels.hpp"
#include "../../util/debug/errors.hpp"
#include "../../util/debug/logs.hpp"

enum UIColorStyle
{
	UIColorStyle_Default,
	UIColorStyle_CustomAll,
	UIColorStyle_CustomBorders
};

extern Config::Bool uiUseTabs;
extern Config::Enum uiColorStyle;
extern Config::Color uiColor;
extern Config::Color uiTextColor;

void ImGui::Custom::WindowManager::UnregisterWindow(std::deque<WindowData>::iterator iter) noexcept
{
	UTIL_ASSERT(iter != _windows.end(), "Window is registered, but we didn't found it??!!");

	UTIL_LABEL_ENTRY(UTIL_XOR(L"Erasing window from list"));
	delete iter->window;

	_windows.erase(iter);
	UTIL_LABEL_OK();

	UTIL_XLOG(L"Window successfully unregistered!");
}

ImGui::Custom::WindowManager::WindowManager() noexcept
{
	UTIL_XLOG(L"Created window manager");
}

ImGui::Custom::WindowManager::~WindowManager() noexcept
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Destroying window manager"));

	for (const auto& data : _windows)
		delete data.window;
		
	UTIL_LABEL_OK();
}

bool ImGui::Custom::WindowManager::IsRegistered(const Window& window) const noexcept
{
	return std::find_if(_windows.begin(), _windows.end(), [window] (const WindowData& data) -> bool
	{
		return *data.window == window;
	}) != _windows.end();
}

bool ImGui::Custom::WindowManager::IsRegistered(std::size_t window_id) const noexcept
{
	return std::find_if(_windows.begin(), _windows.end(), [window_id] (const WindowData& data) -> bool
	{
		return data.id == window_id;
	}) != _windows.end();
}

bool ImGui::Custom::WindowManager::UnregisterWindow(const Window& window) noexcept
{
	UTIL_LOG(UTIL_WFORMAT(
		UTIL_XOR(L"Trying to unregister window ") <<
		Util::ToWideChar(window.GetTitle())
	));

	if (!IsRegistered(window))
	{
		UTIL_XLOG(L"Window is not registered");
		UTIL_DEBUG_ASSERT(false);

		return false;
	}

	UnregisterWindow(std::find_if(_windows.begin(), _windows.end(), [window] (const WindowData& data) -> bool
	{
		return *data.window == window;
	}));

	return true;
}

bool ImGui::Custom::WindowManager::UnregisterWindow(std::size_t window_id) noexcept
{
	UTIL_LOG(UTIL_WFORMAT(
		UTIL_XOR(L"Trying to unregister window id ") <<
		window_id
	));

	if (!IsRegistered(window_id))
	{
		UTIL_XLOG(L"Window is not registered");
		UTIL_DEBUG_ASSERT(false);

		return false;
	}

	UnregisterWindow(std::find_if(_windows.begin(), _windows.end(), [window_id] (const WindowData& data) -> bool
	{
		return data.id == window_id;
	}));

	return true;
}

bool ImGui::Custom::WindowManager::RegisterWindow(Window window, std::size_t* window_id) noexcept
{
	UTIL_LOG(UTIL_WFORMAT(
		UTIL_XOR(L"Registering window ") <<
		Util::ToWideChar(window.GetTitle())
	));

	if (IsRegistered(window))
	{
		UTIL_XLOG(L"Window already registered");
		UTIL_DEBUG_ASSERT(false);

		return false;
	}

	// we run out of limits? what.
	if (_windows.size() == _windows.max_size())
	{
		UTIL_XLOG(L"Window manager run out of limits");

		return false;
	}

	WindowData data;
	UTIL_CHECK_ALLOC(data.window = new Window { window });
	data.id = _lastId++;

	while (IsRegistered(data.id))
		data.id++;

	if (window_id) *window_id = data.id;

	_windows.push_back(data);

	// sort windows
	std::sort(_windows.begin(), _windows.end(), WindowData::Compare);

	UTIL_LOG(UTIL_WFORMAT(
		UTIL_XOR(L"Window has been registered with id ") <<
		data.id
	));
	return true;
}

const ImGui::Custom::Window* ImGui::Custom::WindowManager::GetWindow(std::size_t window_id) const noexcept
{
	auto iter = std::find_if(_windows.begin(), _windows.end(), [window_id] (const WindowData& data) -> bool
	{
		return data.id == window_id;
	});

	return iter == _windows.end()
		? nullptr
		: iter->window;
}

void ImGui::Custom::WindowManager::Render() noexcept
{
	if (ImGui::Begin(UTIL_CXOR("Window Manager"), nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Custom::Variable::Boolean(uiUseTabs);
		ImGui::NewLine();
		ImGui::SetNextItemWidth(120.f);
		ImGui::Custom::Variable::Enum(uiColorStyle);
		
		if (uiColorStyle.GetCurrentItem() != UIColorStyle_Default)
		{
			ImGui::Custom::Variable::Color(uiColor);

			if (uiColorStyle.GetCurrentItem() == UIColorStyle_CustomAll)
				ImGui::Custom::Variable::Color(uiTextColor);
		}

		ImGui::Separator();

		if (ImGui::BeginChild(
			UTIL_CXOR("WNDMGR##ITEMS"),
			ImVec2(250.f, ImGui::GetTextLineHeightWithSpacing() * float(_windows.size() / 2)))
			)
		{
			for (auto& data : _windows)
			{
				bool selected;

				ImGui::PushID(data.id);

				if (data.window->_isOpen)
					selected = ImGui::Selectable(UTIL_CXOR("[-]"), false);
				else
					selected = ImGui::Selectable(UTIL_CXOR("[+]"), true);

				ImGui::SameLine();
				ImGui::TextUnformatted(data.window->_title.c_str());

				if (selected)
					data.window->ToggleOpen();

				ImGui::PopID();
			}
		}
		ImGui::EndChild();
		ImGui::Separator();

		if (ImGui::Button(UTIL_CXOR("Shutdown"), ImVec2(-1.f, 0.f)))
			Main::Shutdown();
	}
	ImGui::End();
}

void ImGui::Custom::WindowManager::RenderWindows() noexcept
{
	static constexpr ImGuiWindowFlags FLAGS
	{
		ImGuiTabBarFlags_FittingPolicyScroll |
		ImGuiTabBarFlags_Reorderable |
		ImGuiTabBarFlags_NoTooltip
	};

	const bool use_tabs = *uiUseTabs;

	if (use_tabs
		? ImGui::Begin(UTIL_CXOR("Brutality")) && ImGui::BeginTabBar(UTIL_CXOR("TABS##BRUTALITY"), FLAGS)
		: true)
	{
		for (auto& data : _windows)
		{
			if (!use_tabs && !data.window->_isOpen)
				continue;

			if (use_tabs
				? ImGui::BeginTabItem(data.window->_title.c_str(), &data.window->_isOpen, data.window->_flags)
				: ImGui::Begin(data.window->_title.c_str(), nullptr, data.window->_flags))
			{
				UTIL_DEBUG_ASSERT(data.window);
				UTIL_DEBUG_ASSERT(data.window->_onWindowRender);

				ImGui::PushID(data.id);
				data.window->_onWindowRender(*data.window);
				ImGui::PopID();

				if (use_tabs)
					ImGui::EndTabItem();
			}

			if (!use_tabs)
				ImGui::End();
		}
	}

	if (use_tabs)
	{
		ImGui::EndTabBar();
		ImGui::End();
	}
}

ImGui::Custom::Window::Window(const std::string& title, ImGuiWindowFlags flags, WindowRenderCallback callback)
	: _title { title }, _flags { flags }, _onWindowRender { callback }, _isOpen { false }
{
	UTIL_DEBUG_ASSERT(callback);
}

ImGui::Custom::Window::Window(const std::string& title, ImGuiWindowFlags flags, WindowRenderCallback callback, std::size_t* id)
	: Window(title, flags, callback)
{
	UTIL_DEBUG_ASSERT(windowManager->RegisterWindow(*this, id));
}
