#pragma once
#include "sdk.hpp"
#include "interfaces.hpp"
#include "viewsetup.hpp"
#include "renderable.hpp"

#include "vector.hpp"
#include "matrix.hpp"

namespace SourceSDK
{
	class CRender
	{
	public:
		const VMatrix& WorldToScreenMatrix();
	};

	SOURCE_SDK_INTERFACE_DECL(CRender, render);
}
