#include "entity.hpp"
#include "entitylist.hpp"
#include "networkable.hpp"

#include "../build.hpp"
#include "../main.hpp"

#include "../util/vmt.hpp"
#include "../util/memory.hpp"

SourceSDK::BaseEntity* SourceSDK::BaseEntity::GetByIndex(int idx) noexcept
{
	return interfaces->entitylist->GetEntity(idx);
}

SourceSDK::BaseEntity* SourceSDK::BaseEntity::GetByHandle(EntityHandle handle) noexcept
{
	const auto networkable { interfaces->entitylist->GetNetworkable(handle) };

	return networkable ? networkable->ToEntity() : nullptr;
}

const char* SourceSDK::BaseEntity::GetClassname()
{
	using GetClassnameFn = const char*(__thiscall*)(void*);

	#if BUILD_GAME_IS_GMOD
	/*
	push    offset aClass   ; "class "
	push    eax             ; char *
	mov     [ebp+var_14], 0
	mov     [ebp+var_20], 0FFh
	mov     [ebp+var_24], 0FFh
	mov     [ebp+var_28], 0FFh
	mov     [ebp+var_2C], 0FFh
	mov     [ebp+var_78], 0
	call    Q_strncpy
	mov     ecx, [ebp+var_C]
	add     esp, 0Ch
	call    GetClassname
	mov     [ebp+var_10], eax
	cmp     byte ptr [eax], 0
	*/
	static const auto getClassname
	{
		Util::GetAbsAddress<GetClassnameFn>
		UTIL_XFIND_PATTERN(
			"client.dll",
			"E8 ?? ?? ?? ?? 89 45 ?? 80 38 00 75",
			0
		)
	};
	#endif

	return getClassname(this);
}

const SourceSDK::Vector3& SourceSDK::BaseEntity::GetAbsOrigin()
{
	// "Memory allocation of RagdollInfo_t failed!\n"
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"client.dll",
			"8B CE 50 8B 06 FF 50 ?? 50 8B 06 8B CE FF 50 ?? 50",
			7
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<Vector3&>(this, offset);
}

const SourceSDK::Angle& SourceSDK::BaseEntity::GetAbsAngles()
{
	// "Memory allocation of RagdollInfo_t failed!\n"
	#if BUILD_GAME_IS_GMOD
	static const auto offset
	{
		(*(std::uint8_t*)UTIL_XFIND_PATTERN(
			"client.dll",
			"8B CE 50 8B 06 FF 50 ?? 50 8B 06 8B CE FF 50 ?? 50",
			15
		)) / 4
	};
	#endif

	return Util::Vmt::CallMethod<Angle&>(this, offset);
}

void SourceSDK::BaseEntity::SetAbsOrigin(const SourceSDK::Vector3& origin)
{
	using SetAbsOriginFn = void(__thiscall*)(void*, const SourceSDK::Vector3&);

	// "PostDataUpdate" in C_BaseEntity::PostDataUpdate
	#if BUILD_GAME_IS_GMOD
	static const auto setAbsOrigin
	{
		Util::GetAbsAddress<SetAbsOriginFn>
			UTIL_XFIND_PATTERN(
				"client.dll",
				"66 89 83 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 8B CF E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 8B CF E8",
				19
			)
	};
	#endif

	setAbsOrigin(this, origin);
}

void SourceSDK::BaseEntity::SetAbsAngle(const SourceSDK::Angle& angles)
{
	using SetAbsAnglesFn = void(__thiscall*)(void*, const SourceSDK::Angle&);

	// "PostDataUpdate" in C_BaseEntity::PostDataUpdate
	#if BUILD_GAME_IS_GMOD
	static const auto setAbsAngles
	{
		Util::GetAbsAddress<SetAbsAnglesFn>
			UTIL_XFIND_PATTERN(
				"client.dll",
				"66 89 83 ?? ?? ?? ?? E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 8B CF E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 8B CF E8",
				31
			)
	};
	#endif

	setAbsAngles(this, angles);
}
