#pragma once
#include "interfaces.hpp"

namespace SourceSDK
{
	class IGameEvent
	{
	public:
		virtual ~IGameEvent() {}

		virtual const char* GetName() const = 0;

		virtual bool IsReliable() const = 0;
		virtual bool IsLocal() const = 0;
		virtual bool IsEmpty(const char* keyName = nullptr) = 0;

		virtual bool GetBool(const char* keyName = nullptr, bool defaultValue = false) = 0;
		virtual int GetInt(const char* keyName = nullptr, int defaultValue = 0) = 0;
		virtual float GetFloat(const char* keyName = nullptr, float defaultValue = 0.0f) = 0;
		virtual const char* GetString(const char* keyName = nullptr, const char* defaultValue = "") = 0;

		virtual void SetBool(const char* keyName, bool value) = 0;
		virtual void SetInt(const char* keyName, int value) = 0;
		virtual void SetFloat(const char* keyName, float value) = 0;
		virtual void SetString(const char* keyName, const char* value) = 0;
	};

	class IGameEventListener
	{
	public:
		virtual	~IGameEventListener() {}

		virtual void FireGameEvent(IGameEvent* event) = 0;
	};

	class IGameEventManager
	{
	public:
		virtual	~IGameEventManager() {}
		virtual int LoadEventsFromFile(const char* filename) = 0;

		virtual void Reset() = 0;

		virtual bool AddListener(IGameEventListener* listener, const char* name, bool bServerSide = false) = 0;
		virtual bool FindListener(IGameEventListener* listener, const char* name) = 0;
		virtual void RemoveListener(IGameEventListener* listener) = 0;

		virtual IGameEvent* CreateEvent(const char* name, bool bForce = false) = 0;

		virtual bool FireEvent(IGameEvent* event, bool bDontBroadcast = false) = 0;
		virtual bool FireEventClientSide(IGameEvent* event) = 0;

		virtual IGameEvent* DuplicateEvent(IGameEvent* event) = 0;
		virtual void FreeEvent(IGameEvent* event) = 0;
		virtual bool SerializeEvent(IGameEvent* event, void** buf) = 0;
		virtual IGameEvent* UnserializeEvent(void** buf) = 0;
	};

	SOURCE_SDK_INTERFACE_DECL(IGameEventManager, gameevents);
}
