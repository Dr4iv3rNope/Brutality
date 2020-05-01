#pragma once
#include "interfaces.hpp"

namespace SourceSDK
{
	struct GlobalVars final
	{
		float realTime;
		int frameCount;
		float absoluteFrameTime;
		float curTime;
		float frameTime;
		int maxClients;
		int tickCount;
		float intervalPerTick;
		float interpolationAmount;
		int simTicksThisFrame;
		int networkProtocol;
		void** saveData;
		bool isClient;
		int timestampNetworkingBase;
		int timestampRandomizeWindow;

		inline float GetFPS() const noexcept
		{
			return 1 / this->frameTime;
		}
	};

	SOURCE_SDK_INTERFACE_DECL(GlobalVars, globals);
}
