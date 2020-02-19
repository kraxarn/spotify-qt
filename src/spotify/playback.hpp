#pragma once

#include "track.hpp"

namespace spt
{
	typedef struct StructPlayback
	{
		int progressMs = 0;
		Track *item = nullptr;
		bool isPlaying = false;
	} Playback;
}