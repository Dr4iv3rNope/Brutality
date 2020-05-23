#pragma once
#include "../build.hpp"

#include "concmdflags.hpp"

#include <cstddef>

namespace SourceSDK
{
	// aka CCommand
	//
	// public/tier1/convar.h:181
	struct ConCmdArgs final
	{
	public:
		static constexpr int COMMAND_MAX_LENGTH = 512;
		static constexpr int COMMAND_MAX_ARG_COUNT = 64;

		int argCount;
		int firstArgLength;
		char argsBuffer[COMMAND_MAX_LENGTH];
		char argvBuffer[COMMAND_MAX_LENGTH];
		const char* args[COMMAND_MAX_ARG_COUNT];
	};

	// aka ConCommandBase
	//
	// public/tier1/convar.h:99
	class IConCommand
	{
	public:
		static constexpr std::size_t SIZE = 72;

		using CallbackFn = void(*)(const ConCmdArgs&);
		using AutoCompleteFn = int(*)(const char*, void*);

		virtual ~IConCommand() {}

		virtual bool IsCommand() const noexcept = 0;
		virtual bool IsFlagSet(ConCmdFlags_ flag) const noexcept = 0;
		virtual void AddFlags(ConCmdFlags flags) noexcept = 0;
		virtual const char* GetName() const noexcept = 0;
		virtual const char* GetHelpString() const noexcept = 0;
		virtual bool IsRegistered() const noexcept = 0;
		virtual int GetDllIdentificator() const noexcept = 0;

	protected:
		virtual void Create() noexcept = 0;
		virtual void Init() noexcept = 0;
	};

	IConCommand* CreateConCommand(const char* name, IConCommand::CallbackFn callback,
		const char* help_string = "", int flags = 0, IConCommand::AutoCompleteFn ac_callback = nullptr);

	class LinkedIConCommand final
	{
	private:
		IConCommand* _concommand;

	public:
		inline LinkedIConCommand(const char* name, IConCommand::CallbackFn callback,
			const char* help_string = "", int flags = 0, IConCommand::AutoCompleteFn ac_callback = nullptr)
		{
			this->_concommand = CreateConCommand(name, callback, help_string, flags, ac_callback);
		}

		inline ~LinkedIConCommand()
		{
			delete _concommand;
		}

		inline IConCommand* operator->() noexcept
		{
			return this->_concommand;
		}
	};
}
