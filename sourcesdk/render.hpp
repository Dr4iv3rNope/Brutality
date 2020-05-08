#pragma once
#include "../valvesdk/interfaces.hpp"

#include "sdk.hpp"
#include "viewsetup.hpp"
#include "renderable.hpp"

#include "vector.hpp"
#include "matrix.hpp"

namespace SourceSDK
{
	class Render
	{
	public:
		const VMatrix& WorldToScreenMatrix();
	};

	VALVE_SDK_INTERFACE_DECL(Render, render);
}
