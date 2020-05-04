#pragma once
#include <string>

namespace Config
{
	extern void Save(const std::wstring& filename);
	extern void Load(const std::wstring& filename);

	extern const std::wstring& GetSettingsPath();
	extern void RegisterWindow() noexcept;
}
