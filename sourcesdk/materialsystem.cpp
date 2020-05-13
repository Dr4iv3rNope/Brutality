#include "materialsystem.hpp"

#include "../build.hpp"

#include "../util/vmt.hpp"

SourceSDK::QueueFriendlyMaterial* SourceSDK::MaterialSystem::CreateQFriendlyMaterial(const char* material_name, KeyValues* key_values)
{
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint32_t*)UTIL_XFIND_PATTERN(
			"engine.dll",
			"8B 0D ?? ?? ?? ?? 57 68 ?? ?? ?? ?? 8B 01 FF 90",
			16
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<QueueFriendlyMaterial*, const char*, KeyValues*>(this, offset, material_name, key_values);
}

void SourceSDK::Material::ColorModulate(float r, float g, float b) noexcept
{
	// "ViewDrawFade"
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"engine.dll",
			"F3 0F 59 C1 F3 0F 11 ?? ?? FF 52",
			11
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<void, float, float, float>(this, offset, r, g, b);
}

void SourceSDK::Material::AlphaModulate(float alpha) noexcept
{
	// "ViewDrawFade"
	#if	BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"engine.dll",
			"F3 0F 59 05 ?? ?? ?? ?? F3 0F 11 ?? ?? FF 52",
			15
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<void, float>(this, offset, alpha);
}

void SourceSDK::Material::SetMaterialVarFlag(MaterialVar var, bool on)
{
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"engine.dll",
			"6A 01 68 00 80 00 00 8B 01 FF 50",
			11
		)) / 4
	};
	#endif

	Util::Vmt::CallMethod<void, MaterialVar, bool>(this, offset, var, on);
}
