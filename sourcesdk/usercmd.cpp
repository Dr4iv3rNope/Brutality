#include "usercmd.hpp"
#include "cvar.hpp"
#include "convar.hpp"

#include "../main.hpp"

float SourceSDK::GetMaxForwardSpeed() noexcept
{
	static IConVar* cl_forwardspeed = interfaces->cvar->FindVar(UTIL_CXOR("cl_forwardspeed"));
	UTIL_DEBUG_ASSERT(cl_forwardspeed);

	return cl_forwardspeed->GetRaw()->floatValue;
}

float SourceSDK::GetMaxSideSpeed() noexcept
{
	static IConVar* cl_sidespeed = interfaces->cvar->FindVar(UTIL_CXOR("cl_sidespeed"));
	UTIL_DEBUG_ASSERT(cl_sidespeed);

	return cl_sidespeed->GetRaw()->floatValue;
}

float SourceSDK::GetMaxUpSpeed() noexcept
{
	static IConVar* cl_upspeed = interfaces->cvar->FindVar(UTIL_CXOR("cl_upspeed"));
	UTIL_DEBUG_ASSERT(cl_upspeed);

	return cl_upspeed->GetRaw()->floatValue;
}
