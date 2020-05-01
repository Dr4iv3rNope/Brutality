#include "entity.hpp"
#include "entitylist.hpp"
#include "sdk.hpp"

#include "../util/vmt.hpp"

SourceSDK::BaseEntity* SourceSDK::BaseEntity::GetByIndex(int idx) noexcept
{
	return entitylist->GetEntity(idx);
}

SourceSDK::BaseEntity* SourceSDK::BaseEntity::GetByHandle(EntityHandle handle) noexcept
{
	return entitylist->GetEntity(handle);
}

const char* SourceSDK::BaseEntity::GetClassname()
{
	using GetClassnameFn = const char*(__thiscall*)(void*);

	#if SOURCE_SDK_IS_GMOD
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
		Util::GetAbsAddress<GetClassnameFn>(UTIL_XFIND_PATTERN(
			"client.dll", "E8 ?? ?? ?? ?? 89 45 ?? 80 38 00 75", 0
		))
	};
	#endif

	return getClassname(this);
}
