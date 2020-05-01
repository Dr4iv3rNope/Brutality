//
// SDK information
//

#pragma once

#define SOURCE_SDK_GAME_GMOD		1 // garry's mod
#define SOURCE_SDK_GAME_CSS			2 // counter strike: source
#define SOURCE_SDK_GAME_CSGO		3 // counter strike: hacker offensive
//#define SOURCE_SDK_GAME_GMOD64	4 // garry's mod x64 build

// current SDK version
#define SOURCE_SDK_GAME SOURCE_SDK_GAME_GMOD

#if SOURCE_SDK_GAME == SOURCE_SDK_GAME_GMOD
#define SOURCE_SDK_IS_GMOD		true
#define SOURCE_SDK_IS_CSS		false
#define SOURCE_SDK_IS_CSGO		false
#elif SOURCE_SDK_GAME == SSDK_GAME_CSS
#define SOURCE_SDK_IS_GMOD		false
#define SOURCE_SDK_IS_CSS		true
#define SOURCE_SDK_IS_CSGO		false
#elif SOURCE_SDK_GAME == SOURCE_SDK_GAME_CSGO
#define SOURCE_SDK_IS_GMOD		false
#define SOURCE_SDK_IS_CSS		false
#define SOURCE_SDK_IS_CSGO		true
#else
#error "SOURCE_SDK_GAME is not valid"
#endif
