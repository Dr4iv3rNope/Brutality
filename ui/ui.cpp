#include "ui.hpp"
#include "onmenurender.hpp"
#include "onoverlayrender.hpp"

#include "../main.hpp"
#include "../shutdown.hpp"

#include <Windows.h>
#include <ShlObj.h>
#include <d3d9.h>
#include <stdexcept>
#include <fstream>
#include <filesystem>

#include "../sourcesdk/inputsystem.hpp"
#include "../sourcesdk/surface.hpp"
#include "../sourcesdk/clientstate.hpp"

#include "../util/strings.hpp"
#include "../util/vmt.hpp"

#include "../util/debug/logs.hpp"
#include "../util/debug/labels.hpp"
#include "../util/debug/assert.hpp"
#include "../util/debug/errors.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

#include "../config/variable.hpp"

enum UIColorStyle
{
	UIColorStyle_Default,
	UIColorStyle_CustomAll,
	UIColorStyle_CustomBorders
};

extern Config::Enum uiColorStyle;
extern Config::Color uiColor;
extern Config::Color uiTextColor;

static bool isMenuOpen{ false };
#ifdef _DEBUG
static bool dbg_initialized { false };
#endif

static HWND gameWindow;
static WNDPROC oldWindowProc;

static Util::Vmt::HookedMethod* oldLockCursor { nullptr };


static LRESULT __stdcall WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	_SHUTDOWN_GUARD;

	extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);

	// credits to penek
	static bool* showCursor = *(bool**) UTIL_XFIND_PATTERN(
		"vguimatsurface.dll",
		"C6 05 ?? ?? ?? ?? 01 83 FA ?? 77",
		2
	);

	enum class CursorState
	{
		None, Show, Hide
	};
	static CursorState state { CursorState::None };

	if (isMenuOpen)
	{
		if (state == CursorState::None)
			state = *showCursor ? CursorState::Show : CursorState::Hide;

		*showCursor = true;
		interfaces->surface->UnlockCursor();
	}
	else
	{
		if (state != CursorState::None)
		{
			*showCursor = state == CursorState::Show;
			
			state = CursorState::None;
		}
	}

	interfaces->inputsystem->EnableInput(!isMenuOpen);

	return CallWindowProcA(oldWindowProc, hWnd, msg, wParam, lParam);
}

static void __fastcall SurfaceLockCursor(void* ecx, void* edx) noexcept
{
	_SHUTDOWN_GUARD;

	if (isMenuOpen)
		return;

	reinterpret_cast<decltype(SurfaceLockCursor)*>
		(oldLockCursor->GetOriginal())(ecx, edx);
}

static void SetupStyle(ImGuiStyle& style = ImGui::GetStyle()) noexcept
{
	//style.Alpha;
	style.WindowPadding = ImVec2(4.f, 4.f);
	style.WindowRounding = 3.f;
	style.WindowBorderSize = 1.f;
	style.WindowMinSize = ImVec2(50.f, 50.f);
	style.WindowTitleAlign = ImVec2(0.f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ChildRounding = 3.f;
	style.ChildBorderSize = 1.f;
	style.PopupRounding = 3.f;
	style.PopupBorderSize = 1.f;
	style.FramePadding = ImVec2(4.f, 2.f);
	style.FrameRounding = 3.f;
	style.FrameBorderSize = 1.f;
	style.ItemSpacing = ImVec2(4.f, 4.f);
	style.ItemInnerSpacing = ImVec2(4.f, 4.f);
	//style.TouchExtraPadding;
	//style.IndentSpacing;
	//style.ColumnsMinSpacing;
	style.ScrollbarSize = 16.f;
	style.ScrollbarRounding = 3.f;
	//style.GrabMinSize;
	style.GrabRounding = 3.f;
	style.TabRounding = 3.f;
	style.TabBorderSize = 1.f;
	//style.ColorButtonPosition;
	//style.ButtonTextAlign;
	//style.SelectableTextAlign;
	//style.DisplayWindowPadding;
	//style.DisplaySafeAreaPadding;
	//style.MouseCursorScale;
	//style.AntiAliasedLines;
	//style.AntiAliasedFill;
	//style.CurveTessellationTol;
	//style.CircleSegmentMaxError;
}

static void SetupDefaultColors(bool default_text_colors = true, ImVec4* colors = ImGui::GetStyle().Colors) noexcept
{
	if (default_text_colors)
	{
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	}

	colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.33f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.33f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.40f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.47f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.33f, 0.33f, 0.33f, 0.47f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.47f, 0.47f, 0.47f, 0.47f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.67f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.73f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.20f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.20f);
	colors[ImGuiCol_Separator] = ImVec4(0.47f, 0.47f, 0.47f, 0.20f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.67f, 0.67f, 0.67f, 0.20f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.20f, 0.20f, 0.20f, 0.33f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.33f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.33f);
	colors[ImGuiCol_Tab] = ImVec4(0.67f, 0.67f, 0.67f, 0.20f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.80f, 0.80f, 0.80f, 0.40f);
	colors[ImGuiCol_TabActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.40f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.20f, 0.20f, 0.20f, 0.20f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.47f, 0.47f, 0.47f, 0.20f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.20f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.33f, 0.33f, 0.33f, 0.20f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

static void SetupCustomTextColors(ImVec4* colors = ImGui::GetStyle().Colors) noexcept
{
	const auto r = uiTextColor.RedF();
	const auto g = uiTextColor.GreenF();
	const auto b = uiTextColor.BlueF();
	const auto a = uiTextColor.AlphaF();

	colors[ImGuiCol_Text] = ImVec4(r, g, b, a);
	colors[ImGuiCol_TextDisabled] = ImVec4(r * 0.50f, g * 0.50f, b * 0.50f, a);
}

static void SetupCustomColors(ImVec4* colors = ImGui::GetStyle().Colors) noexcept
{
	const auto r = uiColor.RedF();
	const auto g = uiColor.GreenF();
	const auto b = uiColor.BlueF();
	const auto a = uiColor.AlphaF();

	colors[ImGuiCol_WindowBg] = ImVec4(r * 0.50f, g * 0.50f, b * 0.50f, a * 0.70f);
	colors[ImGuiCol_ChildBg] = ImVec4(r * 0.50f, g * 0.50f, b * 0.50f, a * 0.70f);
	colors[ImGuiCol_PopupBg] = ImVec4(r * 0.70f, g * 0.70f, b * 0.70f, a * 0.70f);
	colors[ImGuiCol_Border] = ImVec4(r, g, b, a);
	colors[ImGuiCol_BorderShadow] = ImVec4(r * 0.50f, g * 0.50f, b * 0.50f, a * 0.80f);
	colors[ImGuiCol_FrameBg] = ImVec4(r * 0.60f, g * 0.60f, b * 0.60f, a * 0.50f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(r * 0.67f, g * 0.67f, b * 0.67f, a * 0.53f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(r * 0.53f, g * 0.53f, b * 0.53f, a * 0.53f);
	colors[ImGuiCol_TitleBg] = ImVec4(r * 0.80f, g * 0.80f, b * 0.80f, a * 0.50f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(r, g, b, a * 0.70f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(r * 0.40f, g * 0.40f, b * 0.40f, a * 0.30f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(r * 0.86f, g * 0.86f, b * 0.86f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(r * 0.20f, g * 0.20f, b * 0.20f, a * 0.50f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(r * 0.40f, g * 0.40f, b * 0.40f, a * 0.60f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(r * 0.60f, g * 0.60f, b * 0.60f, a * 0.60f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(r, g, b, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(r, g, b, a * 0.60f);
	colors[ImGuiCol_SliderGrab] = ImVec4(r * 0.60f, g * 0.60f, b * 0.60f, a * 0.50f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(r * 0.33f, g * 0.33f, b * 0.33f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(r * 0.87f, g * 0.87f, b * 0.87f, a * 0.60f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(r * 0.80f, g * 0.80f, b * 0.80f, a * 0.80f);
	colors[ImGuiCol_ButtonActive] = ImVec4(r * 0.67f, g * 0.67f, b * 0.67f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(r * 0.80f, g * 0.80f, b * 0.80f, a * 0.80f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(r * 0.60f, g * 0.60f, b * 0.60f, a * 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(r * 0.33f, g * 0.33f, b * 0.33f, a * 0.80f);
	colors[ImGuiCol_Separator] = ImVec4(r * 0.53f, g * 0.53f, b * 0.53f, a * 0.80f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(r * 0.33f, g * 0.33f, b * 0.33f, a * 0.80f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(r, g, b, a * 0.80f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(r * 0.80f, g * 0.80f, b * 0.80f, a * 0.67f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(r * 0.60f, g * 0.60f, b * 0.60f, a * 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(r * 0.33f, g * 0.33f, b * 0.33f, a * 0.67f);
	colors[ImGuiCol_Tab] = ImVec4(r * 0.50f, g * 0.50f, b * 0.50f, a * 0.60f);
	colors[ImGuiCol_TabHovered] = ImVec4(r * 0.70f, g * 0.70f, b * 0.70f, a * 0.60f);
	colors[ImGuiCol_TabActive] = ImVec4(r, g, b, a * 0.60f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(r * 0.80f, g * 0.80f, b * 0.80f, a * 0.80f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(r * 0.53f, g * 0.53f, b * 0.53f, a * 0.80f);
	colors[ImGuiCol_PlotLines] = ImVec4(r * 0.39f, g * 0.39f, b * 0.39f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(r * 0.69f, g * 0.69f, b * 0.69f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(r * 0.30f, g * 0.30f, b * 0.30f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(r * 0.60f, g * 0.60f, b * 0.60f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(r, g, b, a * 0.80f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(r, g, b, a * 0.10f);
	colors[ImGuiCol_NavHighlight] = ImVec4(r * 0.67f, g * 0.67f, b * 0.67f, a * 0.80f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(r, g, b, a * 0.30f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(r * 0.20f, g * 0.20f, b * 0.20f, a * 0.80f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(r * 0.20f, g * 0.20f, b * 0.20f, a * 0.65f);
}

static void SetupCustomBorderColors(ImVec4* colors = ImGui::GetStyle().Colors) noexcept
{
	const auto r = uiColor.RedF();
	const auto g = uiColor.GreenF();
	const auto b = uiColor.BlueF();
	const auto a = uiColor.AlphaF();

	colors[ImGuiCol_Border] = ImVec4(r, g, b, a);
	colors[ImGuiCol_BorderShadow] = ImVec4(r, g, b, a * 0.60f);
	colors[ImGuiCol_Separator] = ImVec4(r * 0.50f, g * 0.50f, b * 0.50f, a * 0.80f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(r * 0.33f, g * 0.33f, b * 0.33f, a * 0.80f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(r, g, b, a * 0.80f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(r * 0.80f, g * 0.80f, b * 0.80f, a * 0.67f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(r * 0.60f, g * 0.60f, b * 0.60f, a * 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(r * 0.33f, g * 0.33f, b * 0.33f, a * 0.67f);
}

static inline const std::string& GetImGuiIniSettings() noexcept
{
	if (auto directory = Util::ToMultiByte(Main::GetLocalPath()) + UTIL_SXOR("imgui");
		!std::filesystem::is_directory(directory))
	{
		UTIL_XLOG(L"Imgui folder not exist, creating it");

		std::filesystem::create_directory(directory);
	}

	static auto ini_path { Util::ToMultiByte(Main::GetLocalPath()) + UTIL_SXOR("imgui\\windows.ini") };

	return ini_path;
}

std::uint32_t UI::GetPreviewColor(std::uint32_t div_power) noexcept
{
	return
		uiColorStyle.GetCurrentItem() != UIColorStyle_CustomBorders &&
		uiColorStyle.GetCurrentItem() != UIColorStyle_Default
		? IM_COL32(uiColor.Red() / div_power, uiColor.Green() / div_power, uiColor.Blue() / div_power, uiColor.Alpha() / div_power)
		: IM_COL32(20, 20, 20, 155);
}

void UI::Initialize()
{
	// UI already initialized! BUG
	UTIL_DEBUG_ASSERT(!dbg_initialized);

	gameWindow = FindWindowA(UTIL_CXOR("Valve001"), nullptr);
	UTIL_ASSERT(gameWindow, "game window not found");

	oldWindowProc = WNDPROC(SetWindowLongA(gameWindow, GWLP_WNDPROC, LONG(WindowProc)));

	ImGui::CreateContext();

	UTIL_ASSERT(ImGui_ImplDX9_Init(interfaces->device), "failed to initialize dx9 ui");
	UTIL_ASSERT(ImGui_ImplWin32_Init(gameWindow), "failed to initialize win32 ui");

	UTIL_CHECK_ALLOC(oldLockCursor = new Util::Vmt::HookedMethod(
		interfaces->surface, interfaces->surface->GetLockCursorIndex()
	));

	oldLockCursor->Initialize(SurfaceLockCursor);

	//
	// setup fonts
	//
	{
		wchar_t* wfont;

		// getting font path
		UTIL_ASSERT(SHGetKnownFolderPath(FOLDERID_Fonts, KF_FLAG_DONT_VERIFY, nullptr, &wfont) == S_OK,
					"Failed to get font path");

		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF, // Basic Latin + Latin Supplement
			0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
			0x2DE0, 0x2DFF, // Cyrillic Extended-A
			0xA640, 0xA69F, // Cyrillic Extended-B
			0,
		};

		ImGui::GetIO().Fonts->AddFontFromFileTTF(
			//Util::ToMultiByte(std::wstring(wfont)).append(UTIL_SXOR("\\Arial.ttf")).c_str(),
			Util::ToMultiByte(std::wstring(wfont)).append(UTIL_SXOR("\\Verdana.ttf")).c_str(),
			14.f,
			nullptr,
			ranges);

		CoTaskMemFree(wfont);
	}

	//
	// setup colors
	//
	SetupDefaultColors();
	
	//
	// setup styles
	//
	SetupStyle();

	//
	// disable auto save settings/logs
	//
	{
		ImGui::GetIO().IniFilename = GetImGuiIniSettings().c_str();
		ImGui::GetIO().LogFilename = nullptr;
	}

	// setup ui variables callback
	static const auto ui_callback = [] (const auto*, Config::VariableAction action)
	{
		switch (uiColorStyle.GetCurrentItem())
		{
			case UIColorStyle_Default:
				SetupDefaultColors();
				break;

			case UIColorStyle_CustomAll:
				SetupCustomColors();
				SetupCustomTextColors();
				break;

			case UIColorStyle_CustomBorders:
				SetupDefaultColors();
				SetupCustomBorderColors();
				break;

		}
	};
	
	uiColorStyle.SetCallback(ui_callback);
	uiColor.SetCallback(ui_callback);
	uiTextColor.SetCallback(ui_callback);

	#ifdef _DEBUG
	dbg_initialized = true;
	#endif
}

void UI::Shutdown() noexcept
{
	// Bug! Tried to shutdown ui, but it's not initialized!
	UTIL_DEBUG_ASSERT(dbg_initialized);

	UTIL_LABEL_ENTRY(UTIL_XOR(L"Shutdown ui"));

	::interfaces->inputsystem->EnableInput(true);
	::interfaces->inputsystem->ResetInputState();

	delete oldLockCursor;

	ImGui_ImplDX9_InvalidateDeviceObjects();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();

	SetWindowLongA(gameWindow, GWLP_WNDPROC, LONG(oldWindowProc));

	#ifdef _DEBUG
	dbg_initialized = false;
	#endif
	UTIL_LABEL_OK();
}

//#define __ENABLE_STYLE_EDITOR
#ifdef __ENABLE_STYLE_EDITOR
// pasted from imgui_demo.cpp
inline void StyleEditor()
{
	if (ImGui::Begin(UTIL_CXOR("Style Editor")))
	{
		//
		// paste begin
		//
		ImGuiStyle& style = ImGui::GetStyle();
		static ImGuiStyle ref_saved_style;
		static ImGuiStyle* ref;

		// Default to using internal storage as reference
		static bool init = true;
		if (init && ref == nullptr)
			ref_saved_style = style;
		init = false;
		if (ref == nullptr)
			ref = &ref_saved_style;

		ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

		// Simplified Settings
		if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
			style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
		{
			bool window_border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &window_border)) style.WindowBorderSize = window_border ? 1.0f : 0.0f;
		}
		ImGui::SameLine();
		{
			bool frame_border = (style.FrameBorderSize > 0.0f); if (ImGui::Checkbox("FrameBorder", &frame_border)) style.FrameBorderSize = frame_border ? 1.0f : 0.0f;
		}
		ImGui::SameLine();
		{
			bool popup_border = (style.PopupBorderSize > 0.0f); if (ImGui::Checkbox("PopupBorder", &popup_border)) style.PopupBorderSize = popup_border ? 1.0f : 0.0f;
		}

		// Save/Revert button
		if (ImGui::Button("Save Ref"))
			*ref = ref_saved_style = style;
		ImGui::SameLine();
		if (ImGui::Button("Revert Ref"))
			style = *ref;
		ImGui::SameLine();
		//HelpMarker("Save/Revert in local non-persistent storage. Default Colors definition are not affected. Use \"Export\" below to save them somewhere.");

		ImGui::Separator();

		if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Sizes"))
			{
				ImGui::Text("Main");
				ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
				ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
				ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
				ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
				ImGui::Text("Borders");
				ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::Text("Rounding");
				ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
				ImGui::Text("Alignment");
				ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
				int window_menu_button_position = style.WindowMenuButtonPosition + 1;
				if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
					style.WindowMenuButtonPosition = window_menu_button_position - 1;
				ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
				ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f"); //HelpMarker("Alignment applies when a button is larger than its text content.");
				ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f"); ImGui::SameLine(); //HelpMarker("Alignment applies when a selectable is larger than its text content.");
				ImGui::Text("Safe Area Padding");	//HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
				ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Colors"))
			{
				static int output_dest = 0;
				static bool output_only_modified = true;
				if (ImGui::Button("Export"))
				{
					if (output_dest == 0)
						ImGui::LogToClipboard();
					else
						ImGui::LogToTTY();
					ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;\n");
					for (int i = 0; i < ImGuiCol_COUNT; i++)
					{
						const ImVec4& col = style.Colors[i];
						const char* name = ImGui::GetStyleColorName(i);
						if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
							ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);\n", name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
					}
					ImGui::LogFinish();
				}
				ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
				ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

				static ImGuiTextFilter filter;
				filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

				static ImGuiColorEditFlags alpha_flags = 0;
				if (ImGui::RadioButton("Opaque", alpha_flags == 0)) {
					alpha_flags = 0;
				} ImGui::SameLine();
				if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_AlphaPreview)) {
					alpha_flags = ImGuiColorEditFlags_AlphaPreview;
				} ImGui::SameLine();
				if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) {
					alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf;
				} ImGui::SameLine();
				//HelpMarker("In the color list:\nLeft-click on colored square to open color picker,\nRight-click to open edit options menu.");

				ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
				ImGui::PushItemWidth(-160);
				for (int i = 0; i < ImGuiCol_COUNT; i++)
				{
					const char* name = ImGui::GetStyleColorName(i);
					if (!filter.PassFilter(name))
						continue;
					ImGui::PushID(i);
					ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
					if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
					{
						// Tips: in a real user application, you may want to merge and use an icon font into the main font, so instead of "Save"/"Revert" you'd use icons.
						// Read the FAQ and docs/FONTS.txt about using icon fonts. It's really easy and super convenient!
						ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save")) ref->Colors[i] = style.Colors[i];
						ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) style.Colors[i] = ref->Colors[i];
					}
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
					ImGui::TextUnformatted(name);
					ImGui::PopID();
				}
				ImGui::PopItemWidth();
				ImGui::EndChild();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Fonts"))
			{
				ImGuiIO& io = ImGui::GetIO();
				ImFontAtlas* atlas = io.Fonts;
				//HelpMarker("Read FAQ and docs/FONTS.txt for details on font loading.");
				ImGui::PushItemWidth(120);
				for (int i = 0; i < atlas->Fonts.Size; i++)
				{
					ImFont* font = atlas->Fonts[i];
					ImGui::PushID(font);
					bool font_details_opened = ImGui::TreeNode(font, "Font %d: \"%s\"\n%.2f px, %d glyphs, %d file(s)", i, font->ConfigData ? font->ConfigData[0].Name : "", font->FontSize, font->Glyphs.Size, font->ConfigDataCount);
					ImGui::SameLine(); if (ImGui::SmallButton("Set as default")) {
						io.FontDefault = font;
					}
					if (font_details_opened)
					{
						ImGui::PushFont(font);
						ImGui::Text("The quick brown fox jumps over the lazy dog");
						ImGui::PopFont();
						ImGui::DragFloat("Font scale", &font->Scale, 0.005f, 0.3f, 2.0f, "%.1f");   // Scale only this font
						ImGui::SameLine(); //HelpMarker("Note than the default embedded font is NOT meant to be scaled.\n\nFont are currently rendered into bitmaps at a given size at the time of building the atlas. You may oversample them to get some flexibility with scaling. You can also render at multiple sizes and select which one to use at runtime.\n\n(Glimmer of hope: the atlas system should hopefully be rewritten in the future to make scaling more natural and automatic.)");
						ImGui::InputFloat("Font offset", &font->DisplayOffset.y, 1, 1, "%.0f");
						ImGui::Text("Ascent: %f, Descent: %f, Height: %f", font->Ascent, font->Descent, font->Ascent - font->Descent);
						ImGui::Text("Fallback character: '%c' (U+%04X)", font->FallbackChar, font->FallbackChar);
						ImGui::Text("Ellipsis character: '%c' (U+%04X)", font->EllipsisChar, font->EllipsisChar);
						const float surface_sqrt = sqrtf((float)font->MetricsTotalSurface);
						ImGui::Text("Texture Area: about %d px ~%dx%d px", font->MetricsTotalSurface, (int)surface_sqrt, (int)surface_sqrt);
						for (int config_i = 0; config_i < font->ConfigDataCount; config_i++)
							if (font->ConfigData)
								if (const ImFontConfig* cfg = &font->ConfigData[config_i])
									ImGui::BulletText("Input %d: \'%s\', Oversample: (%d,%d), PixelSnapH: %d", config_i, cfg->Name, cfg->OversampleH, cfg->OversampleV, cfg->PixelSnapH);
						if (ImGui::TreeNode("Glyphs", "Glyphs (%d)", font->Glyphs.Size))
						{
							// Display all glyphs of the fonts in separate pages of 256 characters
							for (unsigned int base = 0; base <= IM_UNICODE_CODEPOINT_MAX; base += 256)
							{
								int count = 0;
								for (unsigned int n = 0; n < 256; n++)
									count += font->FindGlyphNoFallback((ImWchar)(base + n)) ? 1 : 0;
								if (count > 0 && ImGui::TreeNode((void*)(intptr_t)base, "U+%04X..U+%04X (%d %s)", base, base + 255, count, count > 1 ? "glyphs" : "glyph"))
								{
									float cell_size = font->FontSize * 1;
									float cell_spacing = style.ItemSpacing.y;
									ImVec2 base_pos = ImGui::GetCursorScreenPos();
									ImDrawList* draw_list = ImGui::GetWindowDrawList();
									for (unsigned int n = 0; n < 256; n++)
									{
										ImVec2 cell_p1(base_pos.x + (n % 16) * (cell_size + cell_spacing), base_pos.y + (n / 16) * (cell_size + cell_spacing));
										ImVec2 cell_p2(cell_p1.x + cell_size, cell_p1.y + cell_size);
										const ImFontGlyph* glyph = font->FindGlyphNoFallback((ImWchar)(base + n));
										draw_list->AddRect(cell_p1, cell_p2, glyph ? IM_COL32(255, 255, 255, 100) : IM_COL32(255, 255, 255, 50));
										if (glyph)
											font->RenderChar(draw_list, cell_size, cell_p1, ImGui::GetColorU32(ImGuiCol_Text), (ImWchar)(base + n)); // We use ImFont::RenderChar as a shortcut because we don't have UTF-8 conversion functions available to generate a string.
										if (glyph && ImGui::IsMouseHoveringRect(cell_p1, cell_p2))
										{
											ImGui::BeginTooltip();
											ImGui::Text("Codepoint: U+%04X", base + n);
											ImGui::Separator();
											ImGui::Text("AdvanceX: %.1f", glyph->AdvanceX);
											ImGui::Text("Pos: (%.2f,%.2f)->(%.2f,%.2f)", glyph->X0, glyph->Y0, glyph->X1, glyph->Y1);
											ImGui::Text("UV: (%.3f,%.3f)->(%.3f,%.3f)", glyph->U0, glyph->V0, glyph->U1, glyph->V1);
											ImGui::EndTooltip();
										}
									}
									ImGui::Dummy(ImVec2((cell_size + cell_spacing) * 16, (cell_size + cell_spacing) * 16));
									ImGui::TreePop();
								}
							}
							ImGui::TreePop();
						}
						ImGui::TreePop();
					}
					ImGui::PopID();
				}
				if (ImGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
				{
					ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
					ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
					ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), tint_col, border_col);
					ImGui::TreePop();
				}

				//HelpMarker("Those are old settings provided for convenience.\nHowever, the _correct_ way of scaling your UI is currently to reload your font at the designed size, rebuild the font atlas, and call style.ScaleAllSizes() on a reference ImGuiStyle structure.");
				static float window_scale = 1.0f;
				if (ImGui::DragFloat("window scale", &window_scale, 0.005f, 0.3f, 2.0f, "%.2f"))   // scale only this window
					ImGui::SetWindowFontScale(window_scale);
				ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, 0.3f, 2.0f, "%.2f");      // scale everything
				ImGui::PopItemWidth();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Rendering"))
			{
				ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines); ImGui::SameLine(); //HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");
				ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
				ImGui::PushItemWidth(100);
				ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
				if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;
				ImGui::DragFloat("Circle segment Max Error", &style.CircleSegmentMaxError, 0.01f, 0.10f, 10.0f, "%.2f");
				ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
				ImGui::PopItemWidth();

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::PopItemWidth();
		//
		// paste end
		//
	}
	ImGui::End();
}
#endif

void UI::Draw()
{
	DWORD srgb_default_value;
	interfaces->device->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgb_default_value);
	interfaces->device->SetRenderState(D3DRS_SRGBWRITEENABLE, 0);

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	OnOverlayRender(ImGui::GetBackgroundDrawList(), isMenuOpen);

	if (ImGui::IsKeyPressed(VK_INSERT, false))
	{
		isMenuOpen = !isMenuOpen;

		/* TODO: why we cant save settings?
		
		if (isMenuOpen)
		{
			std::ifstream file(GetImGuiIniSettings());

			if (file)
			{
				std::string content {
					std::istream_iterator<char>(file),
					std::istream_iterator<char>()
				};

				ImGui::LoadIniSettingsFromMemory(content.data(), content.size());
				file.close();
			}
			else
				UTIL_XLOG(L"Failed to load imgui ini settings");
		}
		else
		{
			std::ofstream file(GetImGuiIniSettings());

			if (file)
			{
				std::size_t ini_size;
				const auto ini = ImGui::SaveIniSettingsToMemory(&ini_size);

				file.write(ini, ini_size);
				file.close();
			}
			else
				UTIL_XLOG(L"Failed to save imgui ini settings");
		}*/
	}

	if (isMenuOpen)
	{
		#ifdef __ENABLE_STYLE_EDITOR
		StyleEditor();
		#endif

		OnMenuRender();
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	interfaces->device->SetRenderState(D3DRS_SRGBWRITEENABLE, srgb_default_value);
}

void UI::Reset()
{
	UTIL_XLOG(L"Reset UI devices");

	ImGui_ImplDX9_InvalidateDeviceObjects();
	UTIL_ASSERT(ImGui_ImplDX9_CreateDeviceObjects(), "Failed to create device objects after reset");
}
