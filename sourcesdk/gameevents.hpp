#pragma once
#include "../valvesdk/interfaces.hpp"

namespace SourceSDK
{
	class IGameEvent
	{
	public:
	};

	class IGameEventListener
	{
	public:
		virtual	~IGameEventListener() {}

		virtual void FireGameEvent(IGameEvent* event) = 0;
	};

	class GameEventManager
	{
	public:
	};

	VALVE_SDK_INTERFACE_DECL(GameEventManager, gameevents);
}
