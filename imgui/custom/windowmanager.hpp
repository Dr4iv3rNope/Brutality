#pragma once
#include <string>
#include <functional>
#include <list>

#include "../imgui.h"

namespace ImGui
{
	namespace Custom
	{
		class WindowManager;

		class Window
		{
			friend WindowManager;

		public:
			using WindowRenderCallback = std::function<void(Window&)>;

		private:
			std::string _title;
			bool _isOpen;
			ImGuiWindowFlags _flags;
			WindowRenderCallback _onWindowRender;
			void* _userdata { nullptr };

		public:
			// you'll register window yourself
			Window(const std::string& title, ImGuiWindowFlags flags, WindowRenderCallback callback);

			// window registers themself
			Window(const std::string& title, ImGuiWindowFlags flags, WindowRenderCallback callback, std::size_t* id);

			inline auto GetFlags() const noexcept { return _flags; }

			inline const std::string& GetTitle() const noexcept { return _title; }
			void SetTitle(const std::string& title) noexcept { _title = title; }

			inline bool IsOpen() const noexcept { return _isOpen; }
			inline void SetOpen(bool open) noexcept { _isOpen = open; }
			inline void ToggleOpen() noexcept { _isOpen = !_isOpen; }

			inline const void* GetUserData() const noexcept { return _userdata; }
			inline void* GetUserData() noexcept { return _userdata; }
			inline void SetUserData(void* userdata) noexcept { _userdata = userdata; }

			inline bool operator==(const Window& window) const noexcept
			{
				return _title == window._title;
			}

			inline bool operator!=(const Window& window) const noexcept
			{
				return _title != window._title;
			}
		};

		class WindowManager final
		{
		private:
			struct WindowData
			{
				Window* window; // allocated
				std::size_t id;
			};
			
			std::size_t _lastId { 0 };
			std::list<WindowData> _windows;

			void UnregisterWindow(std::list<WindowData>::iterator iter) noexcept;

		public:
			WindowManager() noexcept;
			~WindowManager() noexcept;

			bool IsRegistered(const Window& window) const noexcept;
			bool IsRegistered(std::size_t window_id) const noexcept;

			bool UnregisterWindow(const Window& window) noexcept;
			bool UnregisterWindow(std::size_t window_id) noexcept;

			bool RegisterWindow(Window window, std::size_t* window_id = nullptr) noexcept;

			// can return nullptr;
			const Window* GetWindow(std::size_t window_id) const noexcept;

			void Render() noexcept;
			void RenderWindows() noexcept;
		};

		extern WindowManager windowManager;
	}
}
