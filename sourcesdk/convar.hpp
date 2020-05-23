#pragma once
#include "concommand.hpp"

namespace SourceSDK
{
	class IConVar;

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
		IConVar* parent;
		
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

	class IConVar : public IConCommand
	{
	public:
		using ChangeCallbackFn = void(*)(IConVar* convar, const char* str_old_value, float old_value);

		static constexpr std::size_t SIZE = 120;

		inline RawConVar* GetRaw() noexcept { return (RawConVar*)this; }

		virtual void SetValue(const char*) noexcept = 0;
		virtual void SetValue(float) noexcept = 0;
		virtual void SetValue(int) noexcept = 0;
		virtual bool Clamp(float&) noexcept = 0;
		virtual bool SetStringValue(const char*, float) noexcept = 0;
	};

	IConVar* CreateConVar(const char* name, const char* default_value, int flags = 0, const char* help_string = "",
		bool min = false, float min_value = 0.0,
		bool max = false, float max_value = 0.0,
		IConVar::ChangeCallbackFn callback = nullptr);

	class LinkedConVar
	{
	private:
		IConVar* _convar;

	public:
		inline LinkedConVar(const char* name, const char* default_value, int flags = 0, const char* help_string = "",
			bool min = false, float min_value = 0.0,
			bool max = false, float max_value = 0.0,
			IConVar::ChangeCallbackFn callback = nullptr)
		{
			this->_convar = CreateConVar(name, default_value, flags, help_string, min, min_value, max, max_value, callback);
		}

		inline ~LinkedConVar()
		{
			delete _convar;
		}

		inline IConVar* operator->() noexcept
		{
			return this->_convar;
		}
	};
}
