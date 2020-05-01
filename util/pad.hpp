#pragma once

// declares unused member (pad)
#define UTIL_PAD(prefix, size) \
	private: char __pad_##prefix[size]; public:
