#include "enginetraceclient.hpp"

#include "../util/vmt.hpp"
#include "../util/xorstr.hpp"

void SourceSDK::EngineTraceClient::TraceRay(const Ray& ray, Mask mask, const ITraceFilter& filter, GameTrace& trace)
{
	// "TraceRay"
	#if SOURCE_SDK_IS_GMOD
	static const auto offset
	{
		VMT_XFIND_METHOD("56 8B F1 89 75 FC 57 A8 01 75 2D 8B 0D", 3)
	};
	#endif

	Util::Vmt::CallMethod<void, const Ray*, Mask, const ITraceFilter*, GameTrace*>
		(this, offset, &ray, mask, &filter, &trace);
}
