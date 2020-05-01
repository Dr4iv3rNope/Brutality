#pragma once
#include "netvars.hpp"
#include "rendermode.hpp"
#include "renderfx.hpp"
#include "collisiongroup.hpp"

#include "vector.hpp"
#include "color.hpp"

namespace SourceSDK
{
	using EntityHandle = void*;
	
	class Renderable;
	class Networkable;

	class BaseEntity
	{
	public:
		static constexpr int RENDERABLE_VMT_OFFSET { 4 };
		static constexpr int NETWORKABLE_VMT_OFFSET { 8 };

		constexpr inline Renderable* ToRenderable() const noexcept
		{
			return (Renderable*)(std::uintptr_t(this) + RENDERABLE_VMT_OFFSET);
		}

		constexpr inline Networkable* ToNetworkable() const noexcept
		{
			return (Networkable*)(std::uintptr_t(this) + NETWORKABLE_VMT_OFFSET);
		}

		static BaseEntity* GetByIndex(int idx) noexcept;
		static BaseEntity* GetByHandle(EntityHandle handle) noexcept;

		SOURCE_SDK_NETVAR(float, GetSimulationTime, "CBaseEntity", "m_flSimulationTime", 0);
		SOURCE_SDK_NETVAR(Vector, GetOrigin, "CBaseEntity", "m_vecOrigin", 0);
		SOURCE_SDK_NETVAR(Angle, GetAngles, "CBaseEntity", "m_angRotation", 0);
		SOURCE_SDK_NETVAR(int, GetModelIndex, "CBaseEntity", "m_nModelIndex", 0);
		SOURCE_SDK_NETVAR(float, GetEffects, "CBaseEntity", "m_fEffects", 0);
		SOURCE_SDK_NETVAR(RenderMode, GetRenderMode, "CBaseEntity", "m_nRenderMode", 0);
		SOURCE_SDK_NETVAR(RenderFx, GetRenderFx, "CBaseEntity", "m_nRenderFX", 0);
		SOURCE_SDK_NETVAR(ByteColor, GetRenderColor, "CBaseEntity", "m_clrRender", 0);
		SOURCE_SDK_NETVAR(int, GetTeam, "CBaseEntity", "m_iTeamNum", 0);
		SOURCE_SDK_NETVAR(CollisionGroup, GetCollisionGroup, "CBaseEntity", "m_CollisionGroup", 0);
		SOURCE_SDK_NETVAR(float, GetElasticity, "CBaseEntity", "m_flElasticity", 0);
		SOURCE_SDK_NETVAR(float, GetShadowCastDistance, "CBaseEntity", "m_flShadowCastDistance", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetOwnerHandle, "CBaseEntity", "m_hOwnerEntity", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetEffectEntityHandle, "CBaseEntity", "m_hEffectEntity", 0);
		SOURCE_SDK_NETVAR(int, GetParentAttachment, "CBaseEntity", "m_iParentAttachment", 0);
		SOURCE_SDK_NETVAR(int, GetTextureFrameIdx, "CBaseEntity", "m_iTextureFrameIndex", 0);
		SOURCE_SDK_NETVAR(bool, IsSimulatedEveryTick, "CBaseEntity", "m_bSimulatedEveryTick", 0);
		SOURCE_SDK_NETVAR(bool, IsAnimatedEveryTick, "CBaseEntity", "m_bAnimatedEveryTick", 0);
		SOURCE_SDK_NETVAR(bool, HasAlternateSorting, "CBaseEntity", "m_bAlternateSorting", 0);
		SOURCE_SDK_NETVAR(int, GetHealth, "CBaseEntity", "m_iHealth", 0);
		SOURCE_SDK_NETVAR(int, GetMaxHealth, "CBaseEntity", "m_iMaxHealth", 0);
		SOURCE_SDK_NETVAR(int, GetSpawnFlags, "CBaseEntity", "m_spawnflags", 0);
		SOURCE_SDK_NETVAR(Vector, GetVelocity, "CBaseEntity", "m_vecVelocity[0]", 0);

		inline BaseEntity* GetOwner() const noexcept
		{
			return GetByHandle(this->GetOwnerHandle());
		}

		inline BaseEntity* GetEffectEntity() const noexcept
		{
			return GetByHandle(this->GetEffectEntityHandle());
		}

		const char* GetClassname();
	};
}
