#pragma once
#include "interfaces.hpp"
#include "gametrace.hpp"

namespace SourceSDK
{
	class Entity;

	enum class TraceType
	{
		Everything
	};

	class ITraceFilter
	{
	public:
		virtual bool ShouldHitEntity(Entity* entity, int mask) const = 0;
		virtual TraceType GetTraceType() const = 0;
	};

	class EngineTraceClient final
	{
	public:
		void TraceRay(const Ray& ray, int mask, const ITraceFilter& filter, GameTrace& trace);
	};

	SOURCE_SDK_INTERFACE_DECL(EngineTraceClient, enginetrace);
}
