#pragma once
#include "../valvesdk/interfaces.hpp"

#include "vector.hpp"
#include "materialvar.hpp"

namespace SourceSDK
{
	class KeyValues;

	class Material final
	{
	public:
		inline void ColorModulate(VecColor4 color) noexcept
		{
			ColorModulate(VecColor3(color.Red(), color.Green(), color.Blue()));
			AlphaModulate(color.Alpha());
		}

		void ColorModulate(VecColor3 color) noexcept;
		void AlphaModulate(float alpha = 1.f) noexcept;

		void SetMaterialVarFlag(MaterialVar var, bool on);
	};

	class MaterialSystem final
	{
	public:
		Material* CreateMaterial(const char* material_name, KeyValues* key_values);
	};

	VALVE_SDK_INTERFACE_DECL(MaterialSystem, materialSystem);
}
