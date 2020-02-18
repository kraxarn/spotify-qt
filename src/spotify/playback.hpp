#pragma once

#include "track.hpp"

namespace spt
{
	typedef struct StructPlayback
	{
		unsigned int progressMs = 0u;
		Track *item = nullptr;
		bool isPlaying = false;
	} Playback;
}