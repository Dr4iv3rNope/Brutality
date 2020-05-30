#include "usercmd.hpp"
#include "cvar.hpp"
#include "convar.hpp"

#include "../main.hpp"

float SourceSDK::GetMaxForwardSpeed() noexcept
{
	static IConVar* cl_forwardspeed = interfaces->cvar->FindVar(UTIL_CXOR("cl_forwardspeed"));
	UTIL_DEBUG_ASSERT(cl_forwardspeed);

	return std::stof(cl_forwardspeed->GetRaw()->stringValue);
}

float SourceSDK::GetMaxSideSpeed() noexcept
{
	static IConVar* cl_sidespeed = interfaces->cvar->FindVar(UTIL_CXOR("cl_sidespeed"));
	UTIL_DEBUG_ASSERT(cl_sidespeed);

	return std::stof(cl_sidespeed->GetRaw()->stringValue);
}

float SourceSDK::GetMaxUpSpeed() noexcept
{
	static IConVar* cl_upspeed = interfaces->cvar->FindVar(UTIL_CXOR("cl_upspeed"));
	UTIL_DEBUG_ASSERT(cl_upspeed);

	return std::stof(cl_upspeed->GetRaw()->stringValue);
}
