#pragma once
#include "concommand.hpp"

namespace SourceSDK
{
	class ConVar;

	struct RawConVar final
	{
	private:
		void* vmt_1;

	public:
		IConCommand* next;
		bool isRegistered;
		const char* convarName;
		const char* helpString;
		int flags;

	private:
		void* vmt_2;

	public:
		ConVar* parent;
		
		const char* defaultValue;
		
		char* stringValue;
		int stringValueLength;

		float floatValue;
		int intValue;

		struct
		{
			bool enabled;
			float value;
		} min, max;

		void* changeCallback;
	};

	class ConVar final : public IConCommand
	{
	public:
		using ChangeCallbackFn = void(*)(ConVar* convar, const char* str_old_value, float old_value);

		static constexpr std::size_t SIZE = 120;

		inline RawConVar* GetRaw() noexcept { return (RawConVar*)this; }

		[[deprecated]] void SetStringValue(const char* value);
	};

	ConVar* CreateConVar(const char* name, const char* default_value, int flags = 0, const char* help_string = "",
		bool min = false, float min_value = 0.0,
		bool max = false, float max_value = 0.0,
		ConVar::ChangeCallbackFn callback = nullptr);

	class LinkedConVar
	{
	private:
		ConVar* _convar;

	public:
		inline LinkedConVar(const char* name, const char* default_value, int flags = 0, const char* help_string = "",
			bool min = false, float min_value = 0.0,
			bool max = false, float max_value = 0.0,
			ConVar::ChangeCallbackFn callback = nullptr)
		{
			this->_convar = CreateConVar(name, default_value, flags, help_string, min, min_value, max, max_value, callback);
		}

		inline ~LinkedConVar()
		{
			delete _convar;
		}

		inline ConVar* operator->() noexcept
		{
			return this->_convar;
		}
	};
}
