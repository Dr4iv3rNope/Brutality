#pragma once
#include "../sourcesdk/modelrender.hpp"

namespace Features
{
	namespace Chams
	{
		extern void Initialize() noexcept;
		extern void Shutdown() noexcept;

		extern bool Render(const SourceSDK::DrawModelState& state,
						   const SourceSDK::ModelRenderInfo& info,
						   SourceSDK::Matrix3x4* boneToWorld) noexcept;
	}
}
