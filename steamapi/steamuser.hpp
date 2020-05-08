#pragma once
#include "../valvesdk/interfaces.hpp"

#include "steamid.hpp"

namespace SteamAPI
{
	enum class UserAppLicenseResult
	{
		HasLicense = 0,			// User has a license for specified app
		DoesNotHaveLicense = 1,	// User does not have a license for the specified app
		NoAuth = 2,				// User has not been authenticated
	};

	using AppID = std::uint32_t;

	using SteamUserHandle = int;
	// interface name: "SteamUser020"
	class SteamUser
	{
	public:
		virtual SteamUserHandle GetSteamUserHandle() = 0;

		// returns true if the Steam client current has a live connection to the Steam servers. 
		// If false, it means there is no active connection due to either a networking issue on the local machine, or the Steam server is down/busy.
		// The Steam client will automatically be trying to recreate the connection as often as possible.
		virtual bool IsLoggedOn() = 0;

		virtual SteamID GetSteamID() = 0;

	private:
		virtual void Unused0() = 0;
		virtual void Unused1() = 0;
		virtual void Unused2() = 0;
		virtual void Unused3() = 0;
		virtual void Unused4() = 0;
		virtual void Unused5() = 0;
		virtual void Unused6() = 0;
		virtual void Unused7() = 0;
		virtual void Unused8() = 0;
		virtual void Unused9() = 0;
		virtual void Unused10() = 0;
		virtual void Unused11() = 0;
		virtual void Unused12() = 0;
		virtual void Unused13() = 0;

	public:
		// After receiving a user's authentication data, and passing it to BeginAuthSession, use this function
		// to determine if the user owns downloadable content specified by the provided AppID.
		virtual UserAppLicenseResult UserHasLicenseForApp(SteamID steamID, AppID appID) = 0;

		// returns true if this users looks like they are behind a NAT device. Only valid once the user has connected to steam 
		// (i.e a SteamServersConnected_t has been issued) and may not catch all forms of NAT.
		virtual bool IsBehindNAT() = 0;

		// set data to be replicated to friends so that they can join your game
		// CSteamID steamIDGameServer - the steamID of the game server, received from the game server by the client
		// uint32 unIPServer, uint16 usPortServer - the IP address of the game server
		virtual void AdvertiseGame(
			SteamID steamIDGameServer,
			std::uint32_t unIPServer,
			std::uint16_t usPortServer
		) = 0;

	private:
		virtual void Unused14() = 0;
		virtual void Unused15() = 0;
		virtual void Unused16() = 0;

	public:
		// gets the Steam Level of the user, as shown on their profile
		virtual int GetPlayerSteamLevel() = 0;

		// gets whether the users phone number is verified 
		virtual bool IsPhoneVerified() = 0;

		// gets whether the user has two factor enabled on their account
		virtual bool IsTwoFactorEnabled() = 0;

		// gets whether the users phone number is identifying
		virtual bool IsPhoneIdentifying() = 0;

		// gets whether the users phone number is awaiting (re)verification
		virtual bool IsPhoneRequiringVerification() = 0;
	};

	VALVE_SDK_INTERFACE_DECL(SteamUser, user);
}
