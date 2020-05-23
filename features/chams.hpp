#pragma once
#include "../sourcesdk/modelrender.hpp"
#include "../sourcesdk/vector.hpp"

#include "../build.hpp"

namespace SourceSDK
{
	class Material;
}

namespace Features
{
	namespace Chams
	{
		extern void Initialize() noexcept;
		extern void Shutdown() noexcept;

		enum class Type
		{
			Disable, Normal, Flat, Shiny,
			#if BUILD_GAME_IS_GMOD
			Glow, SpawnEffect,
			#endif

			_Count
		};

		extern SourceSDK::Material* GetChamsMaterial(Type chams_type) noexcept;

		extern bool RenderChams(const SourceSDK::DrawModelExecuteArgs& args,
								Type vis_type, SourceSDK::VecColor4 vis_color,
								Type occ_type, SourceSDK::VecColor4 occ_color) noexcept;

		extern bool Render(const SourceSDK::DrawModelExecuteArgs& args) noexcept;
	}
}
