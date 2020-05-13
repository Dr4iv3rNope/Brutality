#pragma once
#include "../util/pad.hpp"

#include "vector.hpp"
#include "materialvar.hpp"

namespace SourceSDK
{
	class KeyValues;

	class Material final
	{
	public:
		inline void ColorModulate(VecColor3 color) noexcept
		{
			ColorModulate(color.Red(), color.Green(), color.Blue());
		}

		inline void ColorModulate(VecColor4 color) noexcept
		{
			ColorModulate(color.Red(), color.Green(), color.Blue());
			AlphaModulate(color.Alpha());
		}

		void ColorModulate(float r, float g, float b) noexcept;
		void AlphaModulate(float alpha) noexcept;

		void SetMaterialVarFlag(MaterialVar var, bool on);
	};

	struct QueueFriendlyMaterial final
	{
		UTIL_PAD(vmt, 4);
		Material* material;

		inline const auto* operator->() const noexcept { return material; }
		inline auto* operator->() noexcept { return material; }
	};

	class MaterialSystem final
	{
	public:
		QueueFriendlyMaterial* CreateQFriendlyMaterial(const char* material_name, KeyValues* key_values);
		
		inline Material* CreateMaterial(const char* material_name, KeyValues* key_values)
		{
			if (auto qfMaterial = CreateQFriendlyMaterial(material_name, key_values))
				return qfMaterial->material;
			else
				return nullptr;
		}
	};
}
