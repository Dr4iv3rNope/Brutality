#pragma once
#include <cstdint>
#include <string>

#include "../util/xorstr.hpp"
#include "../util/strings.hpp"

namespace SteamAPI
{
	enum class AccountType
	{
		Invalid = 0,
		Individual = 1,		// single user account
		Multiseat = 2,		// multiseat (e.g. cybercafe) account
		GameServer = 3,		// game server account
		AnonGameServer = 4,	// anonymous game server account
		Pending = 5,		
		ContentServer = 6,
		Clan = 7,
		Chat = 8,
		ConsoleUser = 9,	// Fake SteamID for local PSN account on PS3 or Live account on 360, etc.
		AnonUser = 10,

		_Max
	};

	enum class Universe
	{
		Invalid = 0,
		Public = 1,
		Beta = 2,
		Internal = 3,
		Developer = 4,
		
		_Max
	};

	using SteamID64 = std::uint64_t;
	class SteamID final
	{
	private:
		union
		{
			struct
			{
				std::uint32_t _accountID : 32;			// unique account identifier
				std::uint32_t _accountInstance : 20;	// dynamic instance ID
				AccountType _accountType : 4;			// type of account - can't show as EAccountType, due to signed / unsigned difference
				Universe _universe : 8;					// universe this account belongs to
			};

			SteamID64 _steamid64;
		};

	public:
		inline SteamID(SteamID64 steamid) noexcept
			: _steamid64 { steamid } {}

		// credits to: https://stackoverflow.com/questions/31270278/convert-steamid64-to-steamid
		inline bool ToStringSteamID(std::string& out) const noexcept
		{
			if (IsValid())
			{
				out = UTIL_FORMAT(
					UTIL_XOR("STEAM_") <<
					int(_universe) << ':' <<
					int(_steamid64 & 1) << ':' <<
					int((_steamid64 >> 1) & 0x7FFFFFF)
				);

				return true;
			}
			else
				return false;
		}

		inline bool ToStringSteamID64(std::string& out) const noexcept
		{
			if (IsValid())
			{
				out = std::to_string(_steamid64);
				return true;
			}
			else
				return false;
		}


		inline auto GetSteamID64() const noexcept { return _steamid64; }

		inline auto GetAccountID() const noexcept { return _accountID; }
		inline auto GetAccountInstance() const noexcept { return _accountInstance; }
		inline auto GetAccountType() const noexcept { return _accountType; }
		inline auto GetUniverse() const noexcept { return _universe; }


		inline bool IsGameServerAccount() const noexcept
		{
			return _accountType == AccountType::GameServer ||
				_accountType == AccountType::AnonGameServer;
		}

		inline bool IsPersistentGameServerAccount() const noexcept
		{
			return _accountType == AccountType::GameServer;
		}

		inline bool IsAnonGameServerAccount() const noexcept
		{
			return _accountType == AccountType::AnonGameServer;
		}

		inline bool IsContentServerAccount() const noexcept
		{
			return _accountType == AccountType::ContentServer;
		}

		inline bool IsClanAccount() const noexcept
		{
			return _accountType == AccountType::Clan;
		}

		inline bool IsChatAccount() const noexcept
		{
			return _accountType == AccountType::Chat;
		}

		inline bool IsLobby() const noexcept
		{
			static constexpr int LOBBY_FLAG { (0x000FFFFF + 1) >> 2 };

			return (_accountType == AccountType::Chat)
				&& (_accountInstance & LOBBY_FLAG);
		}

		inline bool IsIndividualAccount() const noexcept
		{
			return _accountType == AccountType::Individual ||
				_accountType == AccountType::ConsoleUser;
		}

		inline bool IsAnonAccount() const noexcept
		{
			return _accountType == AccountType::AnonUser ||
				_accountType == AccountType::AnonGameServer;
		}

		inline bool IsAnonUserAccount() const noexcept
		{
			return _accountType == AccountType::AnonUser;
		}

		inline bool IsConsoleUserAccount() const noexcept
		{
			return _accountType == AccountType::ConsoleUser;
		}

		inline bool IsValid() const noexcept
		{
			static constexpr int SteamUserDefaultInstance { 1 };


			if (_accountType <= AccountType::Invalid || _accountType >= AccountType::_Max)
				return false;

			if (_universe <= Universe::Invalid || _universe >= Universe::_Max)
				return false;

			if (_accountType == AccountType::Individual)
				if (_accountID == 0 || _accountInstance != SteamUserDefaultInstance)
					return false;

			if (_accountType == AccountType::Clan)
				if (_accountID == 0 || _accountInstance != 0)
					return false;

			if (_accountType == AccountType::GameServer)
				if (_accountID == 0)
					return false;

			return true;
		}
	};
}
