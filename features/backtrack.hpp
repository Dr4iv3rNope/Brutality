#pragma once
#include "../sourcesdk/vector.hpp"
#include "../sourcesdk/matrix.hpp"
#include "../sourcesdk/const.hpp"
#include "../sourcesdk/modelrender.hpp"

#include <cstddef>
#include <deque>

struct ImDrawList;

namespace SourceSDK
{
	struct UserCmd;
}

namespace Features
{
	namespace Backtrack
	{
		struct LagRecord final
		{
			SourceSDK::Vector3 origin;

			SourceSDK::Matrix3x4 bones[SourceSDK::MAX_STUDIO_BONES];

			float simulationTime;
		};

		using LagRecordList = std::deque<LagRecord>;

		extern void Update() noexcept;
		extern void Think(SourceSDK::UserCmd* cmd) noexcept;

		extern void DrawBacktrack(ImDrawList* draw) noexcept;
		extern void DrawBacktrack(const SourceSDK::DrawModelExecuteArgs& args) noexcept;
	}
}
