#include "keyvalues.hpp"
#include "../build.hpp"

#include <cstddef>

#include "../util/debug/errors.hpp"

#include "../util/memory.hpp"
#include "../util/pattern.hpp"

void* SourceSDK::KeyValues::Allocate() noexcept
{
	using InitFn = void*(__cdecl*)(std::size_t);

	#if BUILD_GAME_IS_GMOD
	static const auto init
	{
		Util::GetAbsAddress<InitFn> UTIL_XFIND_PATTERN(
			"engine.dll",
			"66 A5 A4 8B 75 ?? 6A 24 E8",
			8
		)
	};
	#endif

	auto heap = init(36);

	UTIL_ASSERT(heap, "Failed to allocate heap for key values!");

	return heap;
}

SourceSDK::KeyValues* SourceSDK::KeyValues::Create(void* allocated, const char* name) noexcept
{
	UTIL_DEBUG_ASSERT(allocated);
	UTIL_DEBUG_ASSERT(name);

	using ConstructorFn = KeyValues*(__thiscall*)(void*, const char*);

	#if BUILD_GAME_IS_GMOD
	static const auto constructor
	{
		Util::GetAbsAddress<ConstructorFn> UTIL_XFIND_PATTERN(
			"engine.dll",
			"85 C0 74 ?? 68 ?? ?? ?? ?? 8B C8 E8 ?? ?? ?? ?? 8B F8 EB ?? 33 FF 8D 85 ?? ?? ?? ?? 8B CF",
			11
		)
	};
	#endif

	return constructor(allocated, name);
}

SourceSDK::KeyValues* SourceSDK::KeyValues::Create(const char* name) noexcept
{
	return Create(Allocate(), name);
}

SourceSDK::KeyValues* SourceSDK::KeyValues::FromBuffer(const char* resource_name, const char* buffer) noexcept
{
	auto keys = Create("");

	UTIL_ASSERT(keys->LoadFromBuffer(resource_name, buffer), "Failed to load keyvalues from buffer!");
	
	return keys;
}

void SourceSDK::KeyValues::SetInt(const char* name, int value)
{
	using SetIntFn = void(__thiscall*)(void*, const char*, int);

	#if BUILD_GAME_IS_GMOD
	static const auto setInt
	{
		Util::GetAbsAddress<SetIntFn> UTIL_XFIND_PATTERN(
			"engine.dll",
			"8B CF 50 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 01 68 ?? ?? ?? ?? 8B CF E8",
			22
		)
	};
	#endif

	return setInt(this, name, value);
}

void SourceSDK::KeyValues::SetString(const char* name, const char* value)
{
	using SetStringFn = void(__thiscall*)(void*, const char*, const char*);

	#if BUILD_GAME_IS_GMOD
	static const auto setString
	{
		Util::GetAbsAddress<SetStringFn> UTIL_XFIND_PATTERN(
			"engine.dll",
			"8D 85 ?? ?? ?? ?? 8B CF 50 68 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A 01",
			14
		)
	};
	#endif

	return setString(this, name, value);
}

bool SourceSDK::KeyValues::LoadFromBuffer(const char* resource_name, const char* buffer, void** filesystem, const char* path)
{
	using LoadFromBufferFn = bool(__thiscall*)(void*, const char*, const char*, void**, const char*, int);

	// "#include is NULL"
	#if BUILD_GAME_IS_GMOD
	static const auto loadFromBuffer
	{
		LoadFromBufferFn UTIL_XFIND_PATTERN(
			"engine.dll",
			"85 DB 75 ?? B0 01 5B 8B E5 5D C2 14 00 56 53 E8",
			-0xD
		)
	};
	#endif

	return loadFromBuffer(this, resource_name, buffer, filesystem, path, 0 /* unk */);
}
