#pragma once
#include "../sourcesdk/modelrender.hpp"

namespace Features
{
	namespace Chams
	{
		extern void Initialize() noexcept;
		extern void Shutdown() noexcept;

		extern bool Render(const SourceSDK::DrawModelExecuteArgs& args) noexcept;
	}
}
