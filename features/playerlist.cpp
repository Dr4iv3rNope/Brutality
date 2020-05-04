#include "playerlist.hpp"
#include "namechanger.hpp"

#include "../main.hpp"

#include "../sourcesdk/globals.hpp"
#include "../sourcesdk/player.hpp"
#include "../sourcesdk/gmodplayer.hpp"
#include "../sourcesdk/networkable.hpp"
#include "../sourcesdk/engineclient.hpp"
#include "../sourcesdk/clientstate.hpp"

#include "../util/xorstr.hpp"
#include "../util/strings.hpp"

#include "../util/debug/logs.hpp"
#include "../util/debug/errors.hpp"

#include "../nlohmann/json.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_stdlib.h"
#include "../imgui/custom/text.hpp"
#include "../imgui/custom/windowmanager.hpp"

#include <deque>
#include <fstream>
#include <optional>

using namespace SourceSDK;
using namespace Features::PlayerList;

constexpr int PLAYER_LIST_VERSION { 1 };

struct PlayerListCachedInfo;
struct PlayerListInfo
{
	bool isCached { false };
	bool isLocalPlayer { false };

	std::string stringify;
	std::string steamid;

	PlayerType type { PlayerType::Normal };
	PlayerInfo engineInfo;

	// disallow self-filling structure
	PlayerListInfo() = delete;

	inline PlayerListInfo(int player_index, const PlayerInfo& info, bool is_local_player = false)
	{
		this->steamid = info.steamid;
		this->engineInfo = info;
		this->isLocalPlayer = is_local_player;
		
		this->stringify = !is_local_player
			? UTIL_FORMAT(
				'[' << (player_index + 1) <<
				UTIL_XOR(" | ") <<
				this->steamid <<
				UTIL_XOR("] ") << info.name
			)
			: UTIL_SXOR("You");
	}

	inline void Uncache() noexcept;
	inline void Cache(const PlayerListCachedInfo& cache) noexcept;
	inline PlayerListCachedInfo* GetCacheInfo() const noexcept;
	inline bool TryLoadFromCache() noexcept;
};

// server player list
using PlayerList = std::deque<std::optional<PlayerListInfo>>;
static PlayerList playerList;

static int selectedPlayer { -1 };

static inline PlayerList::iterator GetPlayerBySteamID(const std::string& steamid) noexcept
{
	return std::find_if(playerList.begin(), playerList.end(), [steamid] (std::optional<PlayerListInfo>& info) -> bool
	{
		return info.has_value() && info.value().steamid == steamid;
	});
}

static inline bool IsPlayerValid(PlayerList::iterator& iter) noexcept
{
	return iter != playerList.end();
}

struct PlayerListCachedInfo
{
	std::string steamid;
	std::string description;
	PlayerType type;

	// some information about this guy

	std::string lastName;
	std::string lastServerIp;

	//
	// updating this structure using playerlist
	//
	inline void Update(const PlayerListInfo& info)
	{
		this->lastName = info.engineInfo.name;
		this->lastServerIp = std::string(clientState->retryAddress, sizeof(clientState->retryAddress));
	}

	inline PlayerListInfo* GetInfo() const noexcept
	{
		if (auto iter = GetPlayerBySteamID(this->steamid);
			IsPlayerValid(iter) && iter->has_value())
			return &iter->value();
		else
			return nullptr;
	}
};

inline void PlayerListInfo::Cache(const PlayerListCachedInfo& cache) noexcept
{
	this->isCached = true;

	this->type = cache.type;
}

// cached/saved player list
using CachedPlayerList = std::deque<PlayerListCachedInfo>;
static CachedPlayerList cachedPlayerList;

static int cachedSelectedPlayer { -1 };

enum class CacheStatus
{
	ThisOutdated,	// need to sync cache 
	FileLoadError,	// failed to load file (this cache is still outdated)
	FileOutdated,	// need to save cache
	FileSaveError,	// failed to save file (the file is still outdated)
	UpToDate
};

static CacheStatus cacheStatus { CacheStatus::ThisOutdated };
static std::string cacheError; // used for CacheStatus::FileLoad/SaveError

static inline CachedPlayerList::iterator GetCachedPlayerBySteamID(const std::string& steamid) noexcept
{
	return std::find_if(cachedPlayerList.begin(), cachedPlayerList.end(), [steamid] (PlayerListCachedInfo& info) -> bool
	{
		return info.steamid == steamid;
	});
}

static inline bool IsCachedPlayerValid(CachedPlayerList::iterator& iter) noexcept
{
	return iter != cachedPlayerList.end();
}

inline PlayerListCachedInfo* PlayerListInfo::GetCacheInfo() const noexcept
{
	if (auto iter = GetCachedPlayerBySteamID(this->steamid); IsCachedPlayerValid(iter))
		return &*iter;
	else
		return nullptr;
}

inline void PlayerListInfo::Uncache() noexcept
{
	if (isCached)
	{
		if (auto iter = GetCachedPlayerBySteamID(this->steamid);
			IsCachedPlayerValid(iter))
		{
			cachedPlayerList.erase(iter);

			this->isCached = false;
		}
	}
}

inline bool PlayerListInfo::TryLoadFromCache() noexcept
{
	if (!this->isCached)
		if (auto cache_info = this->GetCacheInfo(); cache_info)
		{
			this->Cache(*cache_info);
			cache_info->Update(*this);

			return true;
		}

	return false;
}

static bool ImplementPlayer(int idx, PlayerInfo* info = nullptr, bool is_local_player = false)
{
	if (playerList[idx].has_value())
	{
		// if player has been disconnected
		// then remove it from player list
		if (!info)
		{
			playerList[idx] = std::nullopt;
			return true;
		}
		else
		{
			// if player exists and steamid not equal to
			// current player steam id
			if (playerList[idx]->steamid == std::string(info->steamid, MAX_PLAYER_NAME_LENGTH))
			{
				playerList[idx] = PlayerListInfo(idx, *info, is_local_player);
				return true;
			}
		}
	}
	else
	{
		// if player has been connected and
		// player info's not existing in
		// player list, then write it
		if (info)
		{
			playerList[idx] = PlayerListInfo(idx, *info, is_local_player);
			return true;
		}
	}

	return false;
}

bool Features::PlayerList::GetPlayerType(int idx, PlayerType& type)
{
	if (playerList[idx].has_value())
	{
		type = playerList[idx]->type;
		return true;
	}
	else
		return false;
}

static inline auto GetCachedPlayerListPath() noexcept
{
	static auto cachePath
	{
		Main::GetLocalPath() + UTIL_SXOR(L"playerlist.json")
	};

	return cachePath;
}


static inline void ClearCache() noexcept
{
	// if someone has in cache, then reset his type to normal type
	for (auto& cacheInfo : cachedPlayerList)
		if (auto info = cacheInfo.GetInfo(); info)
			info->type = PlayerType::Normal;

	// then cleaning up the cache
	cachedPlayerList.clear();
}

void Features::PlayerList::LoadCache()
{
	ClearCache();

	auto file = std::ifstream(GetCachedPlayerListPath());

	if (file)
	{
		try
		{
			auto root = nlohmann::json::parse(std::string(
				std::istream_iterator<char>(file),
				std::istream_iterator<char>()
			));

			if (root.contains(UTIL_SXOR("version")))
			{
				auto& version = root[UTIL_SXOR("version")];

				if (!version.is_number_unsigned())
				{
					UTIL_DEBUG_OPLOG(throw std::runtime_error(UTIL_XOR("Player list version not a number")));
				}
				else
					if (std::uint32_t int_version = version; int_version > PLAYER_LIST_VERSION)
					{
						UTIL_RUNTIME_ERROR("Player list is not compatible");
					}
			}
			else
				UTIL_DEBUG_OPLOG(throw std::runtime_error(UTIL_XOR("Player list haven't version. Outdated")));

			if (root.contains(UTIL_SXOR("player_list")))
			{
				auto& json_plylist = root[UTIL_SXOR("player_list")];

				if (json_plylist.is_array())
				{
					for (const auto& info : json_plylist)
					{
						#define __PLIST_CHECK_JSON(json_var, var, checkFn) \
							if (info.contains(#json_var)) { \
							auto& json_##json_var = info[UTIL_SXOR(#json_var)]; \
							if (!checkFn) { UTIL_XLOG(L"Failed to parse "#json_var); } else \
							{ cached_info.var = json_##json_var; } }

						//
						// creating and filling PlayerListCachedInfo
						//

						PlayerListCachedInfo cached_info;

						__PLIST_CHECK_JSON(steamid, steamid, json_steamid.is_string());
						__PLIST_CHECK_JSON(last_server_ip, lastServerIp, json_last_server_ip.is_string());
						__PLIST_CHECK_JSON(last_name, lastName, json_last_name.is_string());
						__PLIST_CHECK_JSON(type, type, json_type.is_number_integer());
						__PLIST_CHECK_JSON(description, description, json_description.is_string());

						cachedPlayerList.push_back(cached_info);
					}

					cacheStatus = CacheStatus::UpToDate;
				}
				else
					UTIL_DEBUG_OPLOG(throw std::runtime_error(UTIL_XOR("Tried to parse json, but player_list is not array")));
			}
			else
				UTIL_DEBUG_OPLOG(throw std::runtime_error(UTIL_XOR("Tried to parse json, but player_list is null")));
		}
		catch (const std::exception& ex)
		{
			cacheStatus = CacheStatus::FileLoadError;
			cacheError = UTIL_FORMAT(ex.what() << UTIL_XOR("\nSeems like your file is corrupted or outdated"));

			UTIL_LOG(UTIL_SXOR(L"Load Cache exception: ") + Util::ToWString(cacheError));
		}

		file.close();
	}
	else
	{
		cacheStatus = CacheStatus::FileLoadError;
		cacheError = UTIL_SXOR("Failed to open file");

		UTIL_LOG(UTIL_SXOR(L"Failed to open player list stream ") + GetCachedPlayerListPath());
	}
}

void Features::PlayerList::SaveCache()
{
	auto file = std::ofstream(GetCachedPlayerListPath());

	if (file)
	{
		auto root = nlohmann::json::object({});

		root[UTIL_SXOR("version")] = PLAYER_LIST_VERSION;

		auto& json_plylist = root[UTIL_SXOR("player_list")];
		json_plylist = nlohmann::json::array({});

		for (const auto& cachedInfo : cachedPlayerList)
		{
			//
			// saving PlayerListCachedInfo to json
			//

			auto json_cachedInfo = nlohmann::json::object({});

			json_cachedInfo[UTIL_SXOR("steamid")] = cachedInfo.steamid;
			json_cachedInfo[UTIL_SXOR("last_server_ip")] = cachedInfo.lastServerIp;
			json_cachedInfo[UTIL_SXOR("last_name")] = cachedInfo.lastName;
			json_cachedInfo[UTIL_SXOR("type")] = int(cachedInfo.type);
			json_cachedInfo[UTIL_SXOR("description")] = cachedInfo.description;

			json_plylist.push_back(json_cachedInfo);
		}

		cacheStatus = CacheStatus::UpToDate;
		file << root.dump(4);
		file.close();
	}
	else
	{
		cacheStatus = CacheStatus::FileSaveError;
		cacheError = UTIL_SXOR("Failed to open file");

		UTIL_LOG(UTIL_SXOR(L"Failed to open and save cached player list ") + GetCachedPlayerListPath());
	}
}

void Features::PlayerList::Update()
{
	playerList.resize(globals->maxClients + 1);

	for (auto i = 1; i <= globals->maxClients; i++)
	{
		auto* player = (BasePlayer*)BaseEntity::GetByIndex(i);

		if (!player)
		{
			ImplementPlayer(i - 1, nullptr, player == BasePlayer::GetLocalPlayer());

			continue;
		}

		static PlayerInfo enginePlayerInfo;

		if (!engine->GetPlayerInfo(i, enginePlayerInfo))
			continue;

		ImplementPlayer(i - 1, &enginePlayerInfo, player == BasePlayer::GetLocalPlayer());

		// prevent cache local player
		if (player != BasePlayer::GetLocalPlayer())
			//
			// if player has in cache, then updating cached player's information
			//
			if (auto& playerListInfo = playerList[i - 1];
				playerListInfo.has_value())
				playerListInfo->TryLoadFromCache();
	}
}

static void DrawMenu(ImGui::Custom::Window&) noexcept
{
	if (ImGui::BeginTabBar(UTIL_CXOR("TABS##PLAYER_LIST")))
	{
		if (IsInGame())
		{
			ImGui::PushID(0);
			if (ImGui::BeginTabItem(UTIL_CXOR("Server")))
			{
				ImGui::Columns(2);

				ImGui::Text(UTIL_CXOR("Clients: %i"), globals->maxClients);

				ImGui::ListBox("", &selectedPlayer, [] (void*, int idx, const char** out) -> bool
				{
					static auto no_player { UTIL_SXOR("*No Player*") };

					*out = playerList[idx].has_value()
						? playerList[idx].value().stringify.c_str()
						: no_player.c_str();

					return true;
				}, nullptr, globals->maxClients, 8);

				if (selectedPlayer != -1)
				{
					auto playerListInfoIter = playerList.begin() + selectedPlayer;

					if (playerListInfoIter->has_value())
					{
						auto& playerListInfo = playerListInfoIter->value();

						if (!playerListInfo.isLocalPlayer && !playerListInfo.engineInfo.fakePlayer)
						{
							if (ImGui::Combo
							(
								UTIL_CXOR("Type"),
								(int*)&playerListInfo.type,
								UTIL_CXOR("Normal\0Dangerous\0Friend\0Rage\0")
							))
							{
								bool is_updated { false };
								auto cached_info { GetCachedPlayerBySteamID(playerListInfo.steamid) };

								if (!IsCachedPlayerValid(cached_info))
								{
									//
									// adding new player to cache
									//

									if (playerListInfo.type != PlayerType::Normal)
									{
										UTIL_DEBUG_XLOG(L"Adding new player to cache");

										PlayerListCachedInfo cached_info;
										cached_info.steamid = playerListInfo.steamid;
										cached_info.lastName = std::string(playerListInfo.engineInfo.name, MAX_PLAYER_NAME_LENGTH);
										cached_info.lastServerIp = std::string(clientState->retryAddress, sizeof(clientState->retryAddress));
										cached_info.type = playerListInfo.type;
										playerListInfo.isCached = true;

										cachedPlayerList.push_back(cached_info);
										is_updated = true;
									}
								}
								else
								{
									if (playerListInfo.type == PlayerType::Normal)
									{
										//
										// remove player from cache
										//

										UTIL_DEBUG_XLOG(L"Removing player from cache");

										cachedPlayerList.erase(cached_info);
										playerListInfo.isCached = false;

										is_updated = true;
									}
									else
									{
										//
										// updating cache
										//

										UTIL_DEBUG_LOG(L"Updating cache");

										cached_info->type = playerListInfo.type;

										is_updated = true;
									}
								}

								if (is_updated)
									cacheStatus = CacheStatus::FileOutdated;
							}
						}

						//
						// actions
						//

						if (ImGui::Button(UTIL_CXOR("Stole name")))
							Features::NameChanger::SetName(
								std::string(playerListInfoIter->value().engineInfo.name, MAX_PLAYER_NAME_LENGTH) + UTIL_SXOR("\\t")
							);

						ImGui::NextColumn();

						//
						// playerlist info
						//
						{
							ImGui::Custom::CopiableText(UTIL_CXOR("Entity Index: %i"), selectedPlayer);
							ImGui::Custom::CopiableText(
								UTIL_CXOR("Is Cached: %s"),
								ImGui::Custom::ToTextBoolean(playerListInfoIter->value().isCached).c_str()
							);

							ImGui::NewLine();

							if (auto player = BaseEntity::GetByIndex(selectedPlayer + 1))
							{
								ImGui::Custom::CopiableText(UTIL_CXOR("Steam ID: %s"), playerListInfoIter->value().steamid.c_str());
								ImGui::Custom::CopiableText(UTIL_CXOR("Name: %s"), playerListInfoIter->value().engineInfo.name);
								ImGui::Custom::CopiableText(UTIL_CXOR("User ID: %i"), playerListInfoIter->value().engineInfo.userid);
								ImGui::Custom::CopiableText(
									UTIL_CXOR("Bot?: %s"),
									ImGui::Custom::ToTextBoolean(playerListInfoIter->value().engineInfo.fakePlayer).c_str()
								);
								ImGui::Custom::CopiableText(
									UTIL_CXOR("HLTV: %s"),
									ImGui::Custom::ToTextBoolean(playerListInfoIter->value().engineInfo.isHLTV).c_str()
								);

								ImGui::Custom::CopiableText(
									UTIL_CXOR("Health: %i (Max: %i)"),
									player->GetHealth(),
									player->GetMaxHealth()
								);

								#if SOURCE_SDK_IS_GMOD
								ImGui::Custom::CopiableText(
									UTIL_CXOR("Team: %i"),
									reinterpret_cast<GmodPlayer*>(player)->GetTeam()
								);
								#endif
							}
							else
								ImGui::TextUnformatted(UTIL_CXOR("Failed to get player entity!"));
						}
					}
					else
						ImGui::TextUnformatted(UTIL_CXOR("Player not valid"));
				}
				else
					ImGui::TextUnformatted(UTIL_CXOR("Select player from list above"));

				ImGui::Columns(1);
				ImGui::EndTabItem();
			}
			ImGui::PopID();
		}

		ImGui::PushID(0);
		if (ImGui::BeginTabItem(UTIL_CXOR("Cache")))
		{
			ImGui::Columns(2);

			//
			// draw cache's status
			//
			switch (cacheStatus)
			{
				case CacheStatus::UpToDate:
					ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), UTIL_CXOR("This cache is up to date"));
					break;

				case CacheStatus::FileOutdated:
					ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), UTIL_CXOR("File's cache is outdated"));

					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::TextUnformatted(UTIL_CXOR("You should save cache file"));
						ImGui::EndTooltip();
					}
					break;

				case CacheStatus::ThisOutdated:
					ImGui::TextColored(ImVec4(1.f, 0.5f, 0.6f, 1.f), UTIL_CXOR("Cache is outdated"));

					if (ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::TextUnformatted(UTIL_CXOR("You should load cache file"));
						ImGui::EndTooltip();
					}
					break;

				case CacheStatus::FileLoadError:
					ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), UTIL_CXOR("This cache is outdated. Error occurred"));

					if (!ImGui::IsItemClicked())
					{
						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text(UTIL_CXOR("Error: \"%s\"\n\nClick to reset error"), cacheError.c_str());
							ImGui::EndTooltip();
						}
					}
					else
					{
						cacheError.clear();
						cacheStatus = CacheStatus::ThisOutdated;
					}
					break;

				case CacheStatus::FileSaveError:
					ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), UTIL_CXOR("File's cache is outdated. Error occurred"));

					if (!ImGui::IsItemClicked())
					{
						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text(UTIL_CXOR("Error: \"%s\"\n\nClick to reset error"), cacheError.c_str());
							ImGui::EndTooltip();
						}
					}
					else
					{
						cacheError.clear();
						cacheStatus = CacheStatus::ThisOutdated;
					}
					break;
			}

			if (ImGui::Button(UTIL_CXOR("Save cache")))
				Features::PlayerList::SaveCache();

			ImGui::SameLine();

			if (ImGui::Button(UTIL_CXOR("Load cache")))
				Features::PlayerList::LoadCache();

			ImGui::PushID(1);
			ImGui::ListBox("", &cachedSelectedPlayer, [] (void* data, int idx, const char** out) -> bool
			{
				*out = cachedPlayerList[idx].lastName.c_str();
				return true;
			}, nullptr, cachedPlayerList.size(), 8);
			ImGui::PopID();

			// if selected cache player is not valid then set
			// index to -1 to prevent exception
			if (std::size_t(cachedSelectedPlayer) >= cachedPlayerList.size())
				cachedSelectedPlayer = -1;

			SKIP_THIS_PLAYER:
			if (cachedSelectedPlayer != -1)
			{
				auto& ply = cachedPlayerList[cachedSelectedPlayer];

				if (ImGui::Combo("",
					(int*)&ply.type,
					UTIL_CXOR("Delete this record\0Dangerous\0Friend\0Rage\0")
				))
				{
					auto info = ply.GetInfo();

					// update if player in the server
					if (info) info->type = ply.type;

					// remove player from cache
					if (ply.type == PlayerType::Normal)
					{
						if (info) info->Uncache();

						cachedSelectedPlayer = -1;

						cacheStatus = CacheStatus::FileOutdated;

						// dont draw invalid info below
						goto SKIP_THIS_PLAYER;
					}
				}

				//
				// actions
				//

				if (ImGui::Button(UTIL_CXOR("Stole name")))
					Features::NameChanger::SetName(ply.lastName + UTIL_SXOR("\\t"));

				ImGui::SameLine();

				if (ImGui::Button(UTIL_CXOR("Join last server")))
					engine->ClientCmdUnrestricted((UTIL_SXOR("connect ") + ply.lastServerIp).c_str());

				ImGui::NextColumn();

				ImGui::TextUnformatted(UTIL_CXOR("Description"));
				ImGui::PushID(2);
				ImGui::InputTextMultiline("", &ply.description, ImVec2(260.f, 80.f));
				ImGui::PopID();

				ImGui::NewLine();

				//
				// draw last info
				//

				ImGui::Custom::CopiableText(UTIL_CXOR("Steam ID: %s"), ply.steamid.c_str());
				ImGui::Custom::CopiableText(UTIL_CXOR("Last Name: %s"), ply.lastName.c_str());
				ImGui::Custom::CopiableText(UTIL_CXOR("Last Server IP: %s"), ply.lastServerIp.c_str());
			}

			ImGui::Columns(1);
			ImGui::EndTabItem();
		}
		ImGui::PopID();
	}
		
	ImGui::EndTabBar();
}

void Features::PlayerList::RegisterWindow() noexcept
{
	ImGui::Custom::windowManager.RegisterWindow(
		ImGui::Custom::Window(
			UTIL_SXOR("Player List"),
			0,
			DrawMenu
		)
	);
}
