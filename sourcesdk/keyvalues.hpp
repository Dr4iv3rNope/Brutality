#pragma once

namespace SourceSDK
{
	class KeyValues final
	{
	private:
		static void* Allocate() noexcept;

	public:
		static KeyValues* Create(void* allocated, const char* name) noexcept;
		static KeyValues* Create(const char* name) noexcept;

		void SetInt(const char* name, int value);
		void SetString(const char* name, const char* value);
	};
}
