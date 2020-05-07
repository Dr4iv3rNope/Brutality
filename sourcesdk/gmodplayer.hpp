#pragma once
#include "sdk.hpp"

#if SOURCE_SDK_IS_GMOD
#include "player.hpp"
#include "gmodentity.hpp"

#include "waterlevel.hpp"

namespace SourceSDK
{
	class GmodPlayer : public BasePlayer
	{
	public:
		SOURCE_SDK_NETVAR(int, GetGmodFlags, "CBaseEntity", "m_iGModFlags", 0);
		SOURCE_SDK_NETVAR(bool, GetGmodBool, "CBaseEntity", "m_GMOD_bool", 0);
		SOURCE_SDK_NETVAR(float, GetGmodFloat, "CBaseEntity", "m_GMOD_float", 0);
		SOURCE_SDK_NETVAR(int, GetGmodInt, "CBaseEntity", "m_GMOD_int", 0);
		SOURCE_SDK_NETVAR(Vector3, GetGmodVector, "CBaseEntity", "m_GMOD_Vector", 0);
		SOURCE_SDK_NETVAR(Angle, GetGmodAngle, "CBaseEntity", "m_GMOD_QAngle", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetEntity, "CBaseEntity", "m_GMOD_EHANDLE", 0);
		SOURCE_SDK_NETVAR(bool, CanUseWeaponInVehicle, "CBasePlayer", "m_bUseWeaponsInVehicle", 0);
		SOURCE_SDK_NETVAR(int, GmodPlayerFlags, "CGMOD_Player", "m_iGModPlayerFlags", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetHoveredWidget, "CGMOD_Player", "m_HoveredWidget", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetPressedWidget, "CGMOD_Player", "m_PressedWidget", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetDrivingEntity, "CGMOD_Player", "m_Driving", 0);
		SOURCE_SDK_NETVAR(int, GetDrivingMode, "CGMOD_Player", "m_DrivingMode", 0);
		SOURCE_SDK_NETVAR(int, GetPlayerClass, "CGMOD_Player", "m_PlayerClass", 0);
		SOURCE_SDK_NETVAR(bool, CanZoom, "CGMOD_Player", "m_bCanZoom", 0);
		SOURCE_SDK_NETVAR(bool, CanWalk, "CGMOD_Player", "m_bCanWalk", 0);
		SOURCE_SDK_NETVAR(bool, IsTyping, "CGMOD_Player", "m_bIsTyping", 0);
		SOURCE_SDK_NETVAR(float, GetStepSize, "CGMOD_Player", "m_StepSize", 0);
		SOURCE_SDK_NETVAR(float, GetJumpPower, "CGMOD_Player", "m_JumpPower", 0);
		SOURCE_SDK_NETVAR(Vector3, GetViewOffset, "CGMOD_Player", "m_ViewOffset", 0);
		SOURCE_SDK_NETVAR(Vector3, GetViewOffsetDucked, "CGMOD_Player", "m_ViewOffsetDucked", 0);
		SOURCE_SDK_NETVAR(float, GetGestureEndTime, "CGMOD_Player", "m_fGestureEndTime", 0);
		SOURCE_SDK_NETVAR(VecColor3, GetPlayerColor, "CGMOD_Player", "m_PlayerColor", 0);
		SOURCE_SDK_NETVAR(VecColor3, GetWeaponColor, "CGMOD_Player", "m_WeaponColor", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetHandse, "CGMOD_Player", "m_Hands", 0);
		SOURCE_SDK_NETVAR(WaterLevel, GetWaterLevel, "CGMOD_Player", "m_nWaterLevel", 0);
		SOURCE_SDK_NETVAR(bool, IsSprinting, "CHL2_Player", "m_fIsSprinting", 0);

		SOURCE_SDK_NETVAR(int, GetArmor, "CPlayerResource", "m_iArmor", 0);
		SOURCE_SDK_NETVAR(int, GetTeam, "CPlayerResource", "m_iTeam", 0);
		SOURCE_SDK_NETVAR(bool, IsConnected, "CPlayerResource", "m_bConnected", 0);
		SOURCE_SDK_NETVAR(int, GetDeaths, "CPlayerResource", "m_iDeaths", 0);
		SOURCE_SDK_NETVAR(int, GetScore, "CPlayerResource", "m_iScore", 0);
		SOURCE_SDK_NETVAR(int, GetPing, "CPlayerResource", "m_iPing", 0);
	};
}

#endif
