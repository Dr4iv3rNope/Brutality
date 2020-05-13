#pragma once
#include "vector.hpp"

namespace SourceSDK
{
	class RenderView final
	{
	public:
		void SetColorModulation(const VecColor3& color) noexcept;
		void SetBlend(float alpha) noexcept;
	};
}
