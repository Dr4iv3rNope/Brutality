#pragma once
#include <list>
#include <string>
#include <optional>

#include "../imgui.h"

namespace ImGui
{
	namespace Custom
	{
		class Error final
		{
		private:
			std::string _what;
			ImVec4 _color;

			std::optional<std::string> _description;

		public:
			Error(const std::string& what, const ImVec4& color) noexcept;
			Error(const std::string& what, const ImVec4& color, const std::string& desc) noexcept;

			inline const std::string& What() const noexcept { return _what; }
			inline const ImVec4& Color() const noexcept { return _color; }
			inline const std::optional<std::string>& Description() const noexcept { return _description; }
		};

		using ErrorList = std::list<Error>;

		// if currentError is -1 then it means we have no error
		extern void StatusError(int& current_error, const ErrorList& errors);
	}
}
