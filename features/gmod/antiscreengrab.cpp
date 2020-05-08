#include "antiscreengrab.hpp"

#if SOURCE_SDK_IS_GMOD
#include "../../shutdown.hpp"

#include "../../util/asmhook.hpp"
#include "../../util/pattern.hpp"
#include "../../util/memory.hpp"

#include "../../util/debug/assert.hpp"
#include "../../util/debug/labels.hpp"

#include "../../config/variable.hpp"

#undef xor


extern Config::Bool antiScreenGrabEnable;

static Util::AsmHook* oldRenderCapture;
static Util::AsmHook* oldRenderCapturePixels;

static std::uintptr_t backRenderCaptureAddress { 0 };
static std::uintptr_t backRenderCapturePixelsAddress { 0 };

static UTIL_NAKED_FUNC(renderCapture)
{
	if (antiScreenGrabEnable)
	{
		__asm
		{
			xor edi, edi
			retn
		};
	}
	else
	{
		__asm
		{
			push ebp
			mov ebp, esp
			sub esp, 0x6C
			jmp backRenderCaptureAddress
		};
	}
}

static UTIL_NAKED_FUNC(renderCapturePixels)
{
	if (antiScreenGrabEnable)
	{
		__asm
		{
			xor eax, eax
			retn
		};
	}
	else
	{
		__asm
		{
			push ebp
			mov ebp, esp
			mov eax, [ebp+0x08]
			jmp backRenderCapturePixelsAddress
		};
	}
}

static Shutdown::Element* shutdownElement;

void Features::GarrysMod::AntiScreenGrab::Initialize()
{
	UTIL_LABEL_ENTRY(UTIL_XOR(L"Anti-Screengrab initialize"));

	UTIL_DEBUG_ASSERT(!oldRenderCapture);
	UTIL_DEBUG_ASSERT(!oldRenderCapturePixels);

	//
	// hooking render capture
	//
	// "render.Capture"
	UTIL_CHECK_ALLOC(oldRenderCapture = new Util::AsmHook(
		UTIL_XFIND_PATTERN(
			"client.dll",
			"FF 15 ?? ?? ?? ?? 57 6A 00 6A 00 8D 4D ?? E8",
			-0x29
		), 6
	));

	UTIL_XLOG(L"Initializing Render Capture hook");
	oldRenderCapture->Initialize(renderCapture);
	backRenderCaptureAddress = oldRenderCapture->GetBackAddress();

	//
	// hooking render capture pixels
	//

	/*
	off_1062ECF8    dd offset aCapturepixels ; "CapturePixels"
	...
	dd offset renderCapturePixels
	*/
	UTIL_CHECK_ALLOC(oldRenderCapturePixels = new Util::AsmHook(
		UTIL_XFIND_PATTERN(
			"client.dll",
			"8B 16 8B CE 6A 02 53 57 50 6A 00 6A 00 FF 52 ?? 8B 06",
			-0xA9
		), 6
	));

	UTIL_XLOG(L"Initializing Render CapturePixels hook");
	oldRenderCapturePixels->Initialize(renderCapturePixels);
	backRenderCapturePixelsAddress = oldRenderCapturePixels->GetBackAddress();

	UTIL_CHECK_ALLOC(shutdownElement = new Shutdown::Element(UTIL_XOR(L"Anti-Screengrab"), [] ()
	{
		AntiScreenGrab::Shutdown();
	}));

	UTIL_LABEL_OK();
}

void Features::GarrysMod::AntiScreenGrab::Shutdown()
{
	UTIL_LABEL_ENTRY(L"Anti-Screengrab shutdown");

	UTIL_DEBUG_ASSERT(oldRenderCapture);
	UTIL_DEBUG_ASSERT(oldRenderCapturePixels);

	delete oldRenderCapture;
	delete oldRenderCapturePixels;
	
	UTIL_LABEL_OK();
}
#endif
