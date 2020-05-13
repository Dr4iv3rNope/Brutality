#include "vmt.hpp"
#include "memory.hpp"
#include "strings.hpp"

#include "debug/errors.hpp"

#include <Windows.h>
#include <stdexcept>

Util::Vmt::HookedMethod::HookedMethod(void* object, std::size_t idx)
{
	UTIL_DEBUG_ASSERT(object);
	UTIL_DEBUG_ASSERT(idx <= GetCount(object));

	this->_region = GetVmt(object) + idx;
	this->_original = *this->_region;

	UTIL_LOG(UTIL_WFORMAT(
		UTIL_XOR(L"Preparing to Hook Method ") << std::hex <<
		int(object) << L' ' <<
		int(this->_region) << L' ' <<
		int(this->_original) << L' '
	));
}

Util::Vmt::HookedMethod::~HookedMethod()
{
	if (initialized)
		Shutdown();
}

void Util::Vmt::HookedMethod::Initialize(const void* new_func)
{
	UTIL_DEBUG_ASSERT(!initialized);
	UTIL_DEBUG_ASSERT(new_func);

	UTIL_LOG(UTIL_WFORMAT(
		UTIL_XOR(L"Hooking Method ") << std::hex <<
		int(this->_region) << L' ' <<
		int(this->_original) << L' '
	));

	{
		DWORD temp, old_prot;

		UTIL_ASSERT(
			VirtualProtect(this->_region, sizeof(void*), PAGE_EXECUTE_READWRITE, &old_prot),
			"Failed to change memory protection"
		);

		UTIL_XLOG(L"Setting new pointer");
		memcpy(this->_region, &new_func, sizeof(void*));

		VirtualProtect(this->_region, sizeof(void*), old_prot, &temp);

		initialized = true;
	}
}

void Util::Vmt::HookedMethod::Shutdown() noexcept
{
	UTIL_LOG(UTIL_WFORMAT(
		UTIL_XOR(L"Shutdown Hooked Method ") << std::hex <<
		int(this->_region) << L' ' <<
		int(this->_original)
	));

	UTIL_DEBUG_ASSERT(initialized);

	{
		DWORD temp, old_prot;

		UTIL_ASSERT(
			VirtualProtect(this->_region, sizeof(void*), PAGE_EXECUTE_READWRITE, &old_prot),
			"Failed to change memory protection"
		);

		UTIL_XLOG(L"Setting old pointer");
		memcpy(this->_region, &this->_original, sizeof(void*));

		VirtualProtect(this->_region, sizeof(void*), old_prot, &temp);
	}
}

const void* Util::Vmt::HookedMethod::GetOriginal() const
{
	UTIL_DEBUG_ASSERT(initialized);

	return this->_original;
}

std::size_t Util::Vmt::FindMethod(void* object, const Pattern& bytes, std::size_t cc_count)
{
	UTIL_LOG(UTIL_WFORMAT(
		UTIL_XOR(L"Trying to find method 0x") <<
		std::hex << int(object)
	));

	const auto count = GetCount(object);
	const auto vmt = GetVmt(object);

	UTIL_DEBUG_ASSERT(vmt);

	for (std::size_t i = 0; i < count; i++)
	{
		const auto method_size = Util::GetFuncSize(std::uintptr_t(vmt[i]), cc_count);

		try
		{
			Util::FindPattern(std::uintptr_t(vmt[i]), std::uintptr_t(vmt[i]) + method_size, bytes);

			UTIL_DEBUG_LOG(UTIL_WFORMAT(UTIL_XOR(L"Found method at ") << i));
			return i;
		}
		catch (...)
		{
			continue;
		}
	}

	UTIL_RUNTIME_ERROR("Failed to find method");
}

std::size_t Util::Vmt::GetCount(void* object)
{
	UTIL_DEBUG_ASSERT(object);

	const auto vmt = GetVmt(object);
	UTIL_DEBUG_ASSERT(vmt);

	std::size_t count{ 0 };

	{
		MEMORY_BASIC_INFORMATION s_memoryInfo;

		while (VirtualQuery(vmt[count], &s_memoryInfo, sizeof(MEMORY_BASIC_INFORMATION))
			&& (s_memoryInfo.Protect == PAGE_EXECUTE_READ || s_memoryInfo.Protect == PAGE_EXECUTE_READWRITE))
			count++;
	}

	UTIL_DEBUG_ASSERT(count != 0);
	return count;
}
