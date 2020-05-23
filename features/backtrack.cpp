#include "backtrack.hpp"
#include "playerlist.hpp"
#include "chams.hpp"

#include "../main.hpp"

#include "../sourcesdk/clientstate.hpp"
#include "../sourcesdk/player.hpp"
#include "../sourcesdk/weapon.hpp"
#include "../sourcesdk/entity.hpp"
#include "../sourcesdk/renderable.hpp"
#include "../sourcesdk/studio.hpp"
#include "../sourcesdk/networkable.hpp"
#include "../sourcesdk/globals.hpp"
#include "../sourcesdk/netchannel.hpp"
#include "../sourcesdk/usercmd.hpp"
#include "../sourcesdk/worldtoscreen.hpp"
#include "../sourcesdk/engineclient.hpp"

#include "../util/strings.hpp"

#include "../util/debug/logs.hpp"

#include "../imgui/imgui.h"

#include "../config/variable.hpp"

#include <optional>
#include <mutex>

extern Config::Bool backtrackEnable;
extern Config::Bool backtrackValidateOnUpdate;
extern Config::Bool backtrackCalcBones;
extern Config::LFloat backtrackMaxTime;
extern Config::LFloat backtrackMaxSimTime;
extern Config::Bool backtrackTrackNormals;
extern Config::Bool backtrackTrackDangerous;
extern Config::Bool backtrackTrackFriends;
extern Config::Bool backtrackTrackRages;
extern Config::Bool backtrackDrawDelay;
extern Config::Bool backtrackDrawHeadDots;
extern Config::Color backtrackChamsVisColor;
extern Config::Enum backtrackChamsVisType;
extern Config::Color backtrackChamsOccColor;
extern Config::Enum backtrackChamsOccType;

struct LagPlayerTrack final
{
	int userid;
	SourceSDK::BasePlayer* player;

	Features::Backtrack::LagRecordList records;
};

static inline auto& GetPlayerTracksMutex() noexcept
{
	static std::mutex playerTracksMutex;

	return playerTracksMutex;
}

static inline auto& GetPlayerTracks() noexcept
{
	static std::deque<std::optional<LagPlayerTrack>> playerTracks;

	return playerTracks;
}

static inline auto FindPlayerTrack(int userid) noexcept
{
	return std::find_if(GetPlayerTracks().begin(), GetPlayerTracks().end(), [userid] (std::optional<LagPlayerTrack>& track) -> bool
	{
		return track && userid == track->userid;
	});
}

static bool CreatePlayerTrack(std::size_t player, LagPlayerTrack& new_track) noexcept
{
	new_track.player = (SourceSDK::BasePlayer*)SourceSDK::BaseEntity::GetByIndex(player + 1);

	if (!new_track.player)
		return false;

	// dont track local player
	if (new_track.player == SourceSDK::BasePlayer::GetLocalPlayer())
		return false;

	if (static SourceSDK::PlayerInfo engineInfo;
		interfaces->engine->GetPlayerInfo(player + 1, engineInfo))
		new_track.userid = engineInfo.userid;
	else
		return false;

	return true;
}

static inline float GetUpdateRate() noexcept
{
	static SourceSDK::IConVar* cl_updaterate = interfaces->cvar->FindVar(UTIL_CXOR("cl_updaterate"));
	static SourceSDK::IConVar* sv_minupdaterate = interfaces->cvar->FindVar(UTIL_CXOR("sv_minupdaterate"));
	static SourceSDK::IConVar* sv_maxupdaterate = interfaces->cvar->FindVar(UTIL_CXOR("sv_maxupdaterate"));

	UTIL_DEBUG_ASSERT(cl_updaterate);
	UTIL_DEBUG_ASSERT(sv_minupdaterate);
	UTIL_DEBUG_ASSERT(sv_maxupdaterate);

	return std::clamp(
		std::stof(cl_updaterate->GetRaw()->stringValue),
		std::stof(sv_minupdaterate->GetRaw()->stringValue),
		std::stof(sv_maxupdaterate->GetRaw()->stringValue)
	);
}

static inline float GetInterpAmount() noexcept
{
	static SourceSDK::IConVar* cl_interp = interfaces->cvar->FindVar(UTIL_CXOR("cl_interp"));
	UTIL_DEBUG_ASSERT(cl_interp);

	return std::stof(cl_interp->GetRaw()->stringValue);
}

static inline float GetInterpRatio() noexcept
{
	static SourceSDK::IConVar* cl_interp_ratio = interfaces->cvar->FindVar(UTIL_CXOR("cl_interp_ratio"));
	static SourceSDK::IConVar* sv_client_min_interp_ratio = interfaces->cvar->FindVar(UTIL_CXOR("sv_client_min_interp_ratio"));
	static SourceSDK::IConVar* sv_client_max_interp_ratio = interfaces->cvar->FindVar(UTIL_CXOR("sv_client_max_interp_ratio"));

	UTIL_DEBUG_ASSERT(cl_interp_ratio);
	UTIL_DEBUG_ASSERT(sv_client_min_interp_ratio);
	UTIL_DEBUG_ASSERT(sv_client_max_interp_ratio);

	return std::clamp(
		std::stof(cl_interp_ratio->GetRaw()->stringValue),
		std::stof(sv_client_min_interp_ratio->GetRaw()->stringValue),
		std::stof(sv_client_max_interp_ratio->GetRaw()->stringValue)
	);
}

// used as player->m_fLerpTime
static inline float GetLerpTime() noexcept
{
	static SourceSDK::IConVar* cl_interpolate = interfaces->cvar->FindVar(UTIL_CXOR("cl_interpolate"));
	UTIL_DEBUG_ASSERT(cl_interpolate);

	if (!!std::stoi(cl_interpolate->GetRaw()->stringValue))
	{
		const auto lerp_ratio = GetInterpRatio();

		static SourceSDK::IConVar* cl_interp = interfaces->cvar->FindVar(UTIL_CXOR("cl_interp"));
		UTIL_DEBUG_ASSERT(cl_interp);

		const auto lerp_amount = std::stof(cl_interp->GetRaw()->stringValue);

		return std::max(lerp_amount, lerp_ratio / GetUpdateRate());
	}
	else
		return 0.f;
}

static float GetDelta(float simtime, float lerp = GetLerpTime()) noexcept
{
	float correct = lerp;

	if (auto chan = interfaces->clientstate->netChannel)
	{
		correct += chan->GetLatency(SourceSDK::Flow::Outgoing);
		correct += chan->GetLatency(SourceSDK::Flow::Incoming);
	}

	return correct - (interfaces->globals->GetServerTime() - simtime);
}

static inline bool IsSimulationTimeValid(float simtime, float lerp = GetLerpTime()) noexcept
{
	return std::abs(GetDelta(simtime, lerp)) <= *backtrackMaxTime;
}

bool TrackPlayer(std::size_t player, bool shouldVerify = true) noexcept
{
	UTIL_DEBUG_ASSERT(player <= interfaces->globals->maxClients);

	if (!*backtrackEnable)
		return false;

	if (*backtrackMaxTime == 0.f)
		return false;

	auto _guard = std::lock_guard(GetPlayerTracksMutex());

	auto& track = GetPlayerTracks()[player];

	if (Features::PlayerList::PlayerType type;
		Features::PlayerList::GetPlayerType(player - 1, type))
		switch (type)
		{
			case Features::PlayerList::PlayerType::Normal:
				if (!backtrackTrackNormals)
				{
					track = std::nullopt;
					return false;
				}

				break;

			case Features::PlayerList::PlayerType::Dangerous:
				if (!backtrackTrackDangerous)
				{
					track = std::nullopt;
					return false;
				}

				break;

			case Features::PlayerList::PlayerType::Friend:
				if (!backtrackTrackFriends)
				{
					track = std::nullopt;
					return false;
				}

				break;

			case Features::PlayerList::PlayerType::Rage:
				if (!backtrackTrackRages)
				{
					track = std::nullopt;
					return false;
				}

				break;
		}

	if (!track)
	{
		if (LagPlayerTrack new_track;
			CreatePlayerTrack(player, new_track))
			track = new_track;
		else
			return false;
	}

	if (!track->player)
	{
		track = std::nullopt;
		return false;
	}

	if (track->player->IsDead())
		return false;

	if (track->player->ToNetworkable()->IsDormant())
		return false;

	if (shouldVerify)
	{
		if (!track->records.empty() &&
			track->records.front().simulationTime >= track->player->GetSimulationTime())
			return false;

		if (static SourceSDK::PlayerInfo engineInfo;
			interfaces->engine->GetPlayerInfo(player + 1, engineInfo))
		{
			if (engineInfo.userid != track->userid)
			{
				track = std::nullopt;
				return false;
			}
		}
		else
			return false;
	}

	const auto lerp = GetLerpTime();

	// clear invalid records
	if (!track->records.empty())
	{
		// simulation time must be more or equal to dead time
		// otherwise simulation time is not valid
		const auto deadTime =
			backtrackValidateOnUpdate
			? 0.f
			: interfaces->globals->GetServerTime() - *backtrackMaxSimTime;

		auto iter = std::find_if(track->records.begin(), track->records.end(), [deadTime] (Features::Backtrack::LagRecord& record) -> bool
		{
			return backtrackValidateOnUpdate ? !IsSimulationTimeValid(record.simulationTime) : record.simulationTime < deadTime;
		});
		
		if (iter != track->records.end())
			track->records.erase(iter, track->records.end());
	}

	// add new record
	{
		Features::Backtrack::LagRecord record;
		if (!track->player->ToRenderable()->SetupBones(
			record.bones,
			SourceSDK::MAX_STUDIO_BONES,
			SourceSDK::BoneMask_Anything,
			record.simulationTime + lerp))
			return false;

		record.origin = track->player->GetAbsOrigin();
		record.simulationTime = track->player->GetSimulationTime();

		track->records.push_front(record);
	}

	return true;
}

bool TrackAllPlayers() noexcept
{
	for (auto i = 1; i <= interfaces->globals->maxClients; i++)
		TrackPlayer(i);

	return true;
}

void Features::Backtrack::DrawBacktrack(ImDrawList* draw) noexcept
{
	if (!backtrackEnable)
		return;

	if (!interfaces->clientstate->IsInGame())
		return;

	if (!backtrackDrawDelay && !backtrackDrawHeadDots)
		return;

	auto _guard = std::lock_guard(GetPlayerTracksMutex());

	const auto lerp = GetLerpTime();

	for (auto& track : GetPlayerTracks())
	{
		if (!track)
			continue;

		if (track->player->IsDead())
			continue;

		if (track->player->ToNetworkable()->IsDormant())
			continue;

		for (auto& record : track->records)
		{
			const auto diff = interfaces->globals->GetServerTime() - record.simulationTime;

			if (backtrackDrawDelay)
				if (auto bottom = record.origin.ToScreen())
					if (auto top = SourceSDK::WorldToScreen(record.origin + SourceSDK::Vector3(0.f, 0.f, diff * 20.f)))
						draw->AddLine(
							ImVec2(top->X(), top->Y()),
							ImVec2(bottom->X(), bottom->Y()),
							IM_COL32(255, 0, 0, 255)
						);

			if (backtrackDrawHeadDots)
			{
				if (int head; track->player->ToRenderable()->FindBoneIndex(UTIL_SXOR("head"), head))
					if (SourceSDK::Vector3 bone;
						SourceSDK::GetBonePosition(record.bones, head, bone))
						if (auto screen = bone.ToScreen())
						{
							draw->AddRectFilled(
								ImVec2(screen->X() - 2.f, screen->Y() - 2.f),
								ImVec2(screen->X() + 2.f, screen->Y() + 2.f),
								IM_COL32_WHITE
							);

							if (backtrackValidateOnUpdate ||
								IsSimulationTimeValid(record.simulationTime, lerp))
								draw->AddRect(
									ImVec2(screen->X() - 4.f, screen->Y() - 4.f),
									ImVec2(screen->X() + 4.f, screen->Y() + 4.f),
									IM_COL32(255, 255, 0, 255)
								);
						}
			}
		}
	}
}

void Features::Backtrack::DrawBacktrack(const SourceSDK::DrawModelExecuteArgs& args) noexcept
{
	if (!interfaces->clientstate->IsInGame())
		return;

	if (!backtrackEnable)
		return;

	if (!backtrackChamsVisType.GetCurrentItem() &&
		!backtrackChamsOccType.GetCurrentItem())
		return;

	if (*backtrackMaxTime == 0.f)
		return;

	if (!args.state->renderable ||
		!args.state->renderable->ToEntity()->ToNetworkable()->GetClientClass()->IsPlayer())
		return;

	auto _guard = std::lock_guard(GetPlayerTracksMutex());

	if (std::size_t(args.info->entityIndex) > GetPlayerTracks().size())
		return;

	const auto lerp = GetLerpTime();

	if (auto track = GetPlayerTracks()[args.info->entityIndex - 1];
		track &&
		track->player &&
		track->player->IsAlive() &&
		!track->player->ToNetworkable()->IsDormant())
		for (auto& record : track->records)
		{
			if (!backtrackValidateOnUpdate &&
				!IsSimulationTimeValid(record.simulationTime, lerp))
				continue;

			Chams::RenderChams(SourceSDK::DrawModelExecuteArgs(*args.state, *args.info, record.bones),
							   Features::Chams::Type(backtrackChamsVisType.GetCurrentItem()),
							   SourceSDK::VecColor4(
								   backtrackChamsVisColor.RedF(),
								   backtrackChamsVisColor.GreenF(),
								   backtrackChamsVisColor.BlueF(),
								   backtrackChamsVisColor.AlphaF()
							   ),
							   Features::Chams::Type(backtrackChamsOccType.GetCurrentItem()),
							   SourceSDK::VecColor4(
								   backtrackChamsOccColor.RedF(),
								   backtrackChamsOccColor.GreenF(),
								   backtrackChamsOccColor.BlueF(),
								   backtrackChamsOccColor.AlphaF()
							   ));
		}

	interfaces->modelrender->ForcedMaterialOverride(nullptr);
}

void Features::Backtrack::Update() noexcept
{
	if (!interfaces->clientstate->IsInGame())
	{
		GetPlayerTracks().clear();
		return;
	}

	GetPlayerTracksMutex().lock();
	GetPlayerTracks().resize(interfaces->globals->maxClients + 1);
	GetPlayerTracksMutex().unlock();

	TrackAllPlayers();
}

void Features::Backtrack::Think(SourceSDK::UserCmd* cmd) noexcept
{
	if (!*backtrackEnable)
		return;

	if (*backtrackMaxTime == 0.f)
		return;

	if (!cmd->HasButton(SourceSDK::Input_Attack))
		return;

	auto local_player = SourceSDK::BasePlayer::GetLocalPlayer();

	if (!local_player)
		return;

	if (local_player->IsDead())
		return;
	
	auto weapon = local_player->GetActiveWeapon();

	if (!weapon)
		return;

	if (weapon->GetNextPrimaryAttack() > interfaces->globals->curTime)
		return;

	if (SourceSDK::Vector3 eye_pos;
		local_player->GetEyePosition(eye_pos))
	{
		auto _guard = std::lock_guard(GetPlayerTracksMutex());

		struct
		{
			const LagPlayerTrack* track = nullptr;
			const LagRecord* record = nullptr;
			float fov = std::numeric_limits<float>::max();
		} best;

		const auto lerp = GetLerpTime();

		for (auto& track : GetPlayerTracks())
		{
			if (!track)
				continue;

			if (track->player->IsDead())
				continue;

			if (track->player->ToNetworkable()->IsDormant())
				continue;

			for (auto& record : track->records)
			{
				if (!IsSimulationTimeValid(record.simulationTime, lerp))
					break; // we will not get valid records after this

				auto pos = record.origin;

				if (backtrackCalcBones)
					if (int head; track->player->ToRenderable()->FindBoneIndex(UTIL_SXOR("head"), head))
						SourceSDK::GetBonePosition(record.bones, head, pos);

				const auto rel = SourceSDK::GetRelativeAngle(eye_pos, pos);
				const auto fov = std::hypot(rel.Pitch(), rel.Yaw());

				if (fov < best.fov)
				{
					best.track = &*track;
					best.record = &record;
					best.fov = fov;
				}
			}
		}

		if (best.track && best.record)
		{
			UTIL_DEBUG_LOG(UTIL_WFORMAT(
				"backtrack found" <<
				"\nrec: [" << best.record->origin.X() << ' ' << best.record->origin.Y() << ' ' << best.record->origin.Z() <<
				"\ncur: [" << best.track->player->GetAbsOrigin().X() << ' ' << best.track->player->GetAbsOrigin().Y() << ' ' << best.track->player->GetAbsOrigin().Z() <<
				"\nticks changed from " << interfaces->globals->tickCount << " to " << interfaces->globals->TimeToTicks(best.record->simulationTime + lerp)
			));

			best.track->player->SetAbsOrigin(best.record->origin);

			cmd->tickCount = interfaces->globals->TimeToTicks(best.record->simulationTime + lerp);
		}
	}
}
