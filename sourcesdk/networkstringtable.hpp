#pragma once

namespace SourceSDK
{
	using TableID = int;

	// public/networkstringtabledefs.h:32
	class INetworkStringTable
	{
	private:
		virtual ~INetworkStringTable() = 0;

	public:
		virtual const char* GetTableName(void) const = 0;
		virtual TableID GetTableId(void) const = 0;
		virtual int GetNumStrings(void) const = 0;
		virtual int GetMaxStrings(void) const = 0;
		virtual int GetEntryBits(void) const = 0;
		virtual void SetTick(int tick) = 0;
		virtual bool ChangedSinceTick(int tick) const = 0;
		virtual int AddString(bool is_server, const char* value, int length = -1, const void* userdata = 0) = 0;
		virtual const char* GetString(int string_number) const = 0;
		virtual void SetStringUserData(int string_number, int length, const void* userdata) = 0;
		virtual const void* GetStringUserData(int string_number, int* length) const = 0;
		virtual int FindStringIndex(char const* string) const = 0;
		virtual void SetStringChangedCallback(void* object, void* change_func) = 0;
	};
}
