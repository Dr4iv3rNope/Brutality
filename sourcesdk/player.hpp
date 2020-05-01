#pragma once
#include "entity.hpp"
#include "lifestate.hpp"
#include "observermode.hpp"
#include "const.hpp"
#include "flags.hpp"

#include <array>

namespace SourceSDK
{
	class BaseWeapon;

	class BasePlayer : public BaseEntity
	{
	public:
		static BasePlayer* GetLocalPlayer(); // can be null

		BaseWeapon* GetActiveWeapon(); // can be null

		SOURCE_SDK_NETVAR(float, GetFov, "CBasePlayer", "m_iFOV", 0);
		SOURCE_SDK_NETVAR(float, GetFovStart, "CBasePlayer", "m_iFOVStart", 0);
		SOURCE_SDK_NETVAR(float, GetFovTime, "CBasePlayer", "m_flFOVTime", 0);
		SOURCE_SDK_NETVAR(float, GetDefaultFov, "CBasePlayer", "m_iDefaultFOV", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetZoomOwnerHandle, "CBasePlayer", "m_hZoomOwner", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetVehicleHandle, "CBasePlayer", "m_hVehicle", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetUseEntityHandle, "CBasePlayer", "m_hUseEntity", 0);
		SOURCE_SDK_NETVAR(LifeState, GetLifeState, "CBasePlayer", "m_lifeState", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetActiveWeaponHandle, "CBaseCombatCharacter", "m_hActiveWeapon", 0);

		using WeaponArray = std::array<EntityHandle, MAX_WEAPONS>;
		SOURCE_SDK_NETVAR(WeaponArray, GetWeapons, "CBaseCombatCharacter", "m_hMyWeapons", 0);

		// same as
		// ply->GetLifeState() == LifeState::Alive
		inline bool IsAlive() const noexcept { return this->GetLifeState() == LifeState::Alive; }

		// same as
		// ply->GetLifeState() != LifeState::Alive
		// !ply->IsAlive()
		inline bool IsDead() const noexcept { return !IsAlive(); }

		SOURCE_SDK_NETVAR(int, GetBonusProgress, "CBasePlayer", "m_iBonusProgress", 0);
		SOURCE_SDK_NETVAR(int, GetBonusChallenge, "CBasePlayer", "m_iBonusChallenge", 0);
		SOURCE_SDK_NETVAR(float, GetMaxSpeed, "CBasePlayer", "m_flMaxspeed", 0);
		SOURCE_SDK_NETVAR(int, GetFlags, "CBasePlayer", "m_fFlags", 0);
		SOURCE_SDK_NETVAR(ObserverMode, GetObserverMode, "CBasePlayer", "m_iObserverMode", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetObserverTargetHandle, "CBasePlayer", "m_hObserverTarget", 0);
		SOURCE_SDK_NETVAR(EntityHandle, GetViewModelHandle, "CBasePlayer", "m_hViewModel[0]", 0);

		inline BaseEntity* GetZoomOwner() const noexcept
		{
			return BaseEntity::GetByHandle(this->GetZoomOwnerHandle());
		}

		inline BaseEntity* GetVehicle() const noexcept
		{
			return BaseEntity::GetByHandle(this->GetVehicleHandle());
		}

		inline BaseEntity* GetUseEntity() const noexcept
		{
			return BaseEntity::GetByHandle(this->GetUseEntityHandle());
		}

		inline BaseEntity* GetObserverTarget() const noexcept
		{
			return BaseEntity::GetByHandle(this->GetObserverTargetHandle());
		}

		inline BaseEntity* GetViewModel() const noexcept
		{
			return BaseEntity::GetByHandle(this->GetViewModelHandle());
		}

		inline bool IsOnGround() const noexcept { return this->GetFlags() & int(Flags::OnGround); }
		inline bool IsDucking() const noexcept { return this->GetFlags() & int(Flags::Ducking); }
		inline bool IsInWater() const noexcept { return this->GetFlags() & int(Flags::InWater); }
		inline bool IsSwim() const noexcept { return this->GetFlags() & int(Flags::Swim); }
		inline bool IsInRain() const noexcept { return this->GetFlags() & int(Flags::InRain); }
		inline bool IsFrozen() const noexcept { return this->GetFlags() & int(Flags::Frozen); }
		inline bool IsFly() const noexcept { return this->GetFlags() & int(Flags::Fly); }
		inline bool IsFakeClient() const noexcept { return this->GetFlags() & int(Flags::FakeClient); }
		inline bool IsClient() const noexcept { return this->GetFlags() & int(Flags::Client); }
	};
}
