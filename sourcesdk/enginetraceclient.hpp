#pragma once
#include "../valvesdk/interfaces.hpp"

#include "gametrace.hpp"
#include "bspflags.hpp"

namespace SourceSDK
{
	class BaseEntity;

	enum class TraceType
	{
		Everything
	};

	class ITraceFilter
	{
	public:
		virtual bool ShouldHitEntity(BaseEntity* entity, Mask mask) const = 0;
		virtual TraceType GetTraceType() const = 0;
	};

	class BasicTraceFilter : public ITraceFilter
	{
	private:
		BaseEntity* _ignore;

	public:
		inline BasicTraceFilter(BaseEntity* ignore) noexcept
			: _ignore { ignore } {}

		virtual bool ShouldHitEntity(BaseEntity* entity, Mask) const override
		{
			return entity != _ignore;
		}

		virtual TraceType GetTraceType() const override
		{
			return TraceType::Everything;
		}
	};

	class EngineTraceClient final
	{
	public:
		void TraceRay(const Ray& ray, Mask mask, const ITraceFilter& filter, GameTrace& trace);
	};
}
