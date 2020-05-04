#pragma once
#include <string>

namespace Features
{
	namespace NameChanger
	{
		extern void SetName(const std::string& new_name) noexcept;
		
		extern void Update() noexcept;

		extern void Initialize() noexcept;
		extern void RegisterWindow() noexcept;
	}
}
