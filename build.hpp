//
// Build information
//

#pragma once

#define BUILD_GAME_GMOD		1 // garry's mod
#define BUILD_GAME_CSS		2 // counter strike: source
#define BUILD_GAME_CSGO		3 // counter strike: hacker offensive

// for what game we building cheat?
#define BUILD_GAME BUILD_GAME_GMOD

#if BUILD_GAME == BUILD_GAME_GMOD
#define BUILD_GAME_IS_GMOD		true
#define BUILD_GAME_IS_CSS		false
#define BUILD_GAME_IS_CSGO		false
#elif BUILD_GAME == BUILD_GAME_CSS
#define BUILD_GAME_IS_GMOD		false
#define BUILD_GAME_IS_CSS		true
#define BUILD_GAME_IS_CSGO		false
#elif BUILD_GAME == BUILD_GAME_CSGO
#define BUILD_GAME_IS_GMOD		false
#define BUILD_GAME_IS_CSS		false
#define BUILD_GAME_IS_CSGO		true
#else
#error "BUILD_GAME is not valid"
#endif
