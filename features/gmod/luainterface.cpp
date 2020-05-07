#include "luainterface.hpp"

#if SOURCE_SDK_IS_GMOD
#include "../../shutdown.hpp"

#include "../../config/variable.hpp"

#include "../../util/xorstr.hpp"
#include "../../util/vmt.hpp"
#include "../../util/strings.hpp"

#include "../../util/debug/logs.hpp"
#include "../../util/debug/labels.hpp"

std::size_t Features::GarrysMod::LuaInterface::GetRunStringIndex()
{
	/*
	push    0
	push    0
	push    400h
	push    eax
	mov     byte_107229CD, 1
	call    sub_10401A40
	mov     ecx, luaInterface
	lea     edx, [ebp+lua_code_str]
	push    1
	push    1
	mov     [ebp+var_4], 0
	mov     eax, [ecx]
	push    edx
	push    offset empty_string
	push    offset aLuacmd  ; "LuaCmd"
	mov     eax, [eax+170h]
	call    eax
	test    al, al
	jnz     short loc_1027CBE6
	*/
	static const auto offset
	{
		(*(int*)UTIL_XFIND_PATTERN(
			"client.dll", "52 68 ?? ?? ?? ?? 68 ?? ?? ?? ?? 8B 80", 13
		)) / 4
	};

	return offset;
}

static Util::Vmt::HookedMethod* oldRunString { nullptr };

extern Config::Bool preventRunString;

static bool __fastcall RunString(void* ecx, void* edx,
								 const char* id, const char* unk0, const char* code,
								 bool unk1, bool unk2) noexcept
{
	if (preventRunString)
		return false;
	else
		return reinterpret_cast<decltype(::RunString)*>
		(oldRunString->GetOriginal())
		(ecx, edx, id, unk0, code, unk1, unk2);
}

static Main::ShutdownElement* shutdownElement;

bool Features::GarrysMod::LuaInterface::IsInitialized() noexcept { return oldRunString; }
bool Features::GarrysMod::LuaInterface::TryToInitialize()
{
	if (oldRunString)
		return false;

	if (!*luaInterface)
		return false;

	UTIL_LABEL_ENTRY(UTIL_XOR(L"Initializing RunString hook"));

	UTIL_CHECK_ALLOC(oldRunString = new Util::Vmt::HookedMethod(*luaInterface, GetRunStringIndex()));
	oldRunString->Initialize(::RunString);

	UTIL_CHECK_ALLOC(shutdownElement = new Main::ShutdownElement(UTIL_XOR(L"LuaInterface"), [] ()
	{
		LuaInterface::Shutdown();
	}));

	UTIL_LABEL_OK();
	return true;
}

void Features::GarrysMod::LuaInterface::Shutdown()
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Initializing RunString hook"));
	UTIL_DEBUG_ASSERT(oldRunString);

	delete oldRunString;
	UTIL_LABEL_OK();
}

bool Features::GarrysMod::LuaInterface::RunString(const char* id, const char* code) noexcept
{
	static bool(__thiscall* function)(void*, const char*, const char*, const char*, bool, bool) = nullptr;

	UTIL_LABEL_ENTRY(UTIL_WFORMAT(L"RunString " << id << ' ' << code));
	
	if (!function)
	{
		if (!oldRunString)
		{
			// uh, we trying to run string but hook not installed
			// whatever, we'll run it anyway, but this mistake must be fixed!
			UTIL_DEBUG_XLOG(L"Tried to RunString, but RunString not hooked!");

			function = (decltype(function))Util::Vmt::GetMethod(this, GetRunStringIndex());
		}
		else
			function = (decltype(function))oldRunString->GetOriginal();

		UTIL_ASSERT(function, "Failed to get old RunString!");
	}

	UTIL_XLOG(L"Running original RunString");
	auto result = function(this, id, "", code, true, true);	
	UTIL_LABEL_OK();

	return result;
}
#endif
