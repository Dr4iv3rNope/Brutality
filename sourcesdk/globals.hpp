#pragma once

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

		int TimeToTicks(float time) noexcept;
		float TicksToTime(int ticks) noexcept;
		float GetServerTime() noexcept;
	};
}
