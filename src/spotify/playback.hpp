#pragma once

#include "track.hpp"

namespace spt
{
	struct Playback
	{
		unsigned int progressMs = 0u;
		Track *item = nullptr;
		bool isPlaying = false;
	};
}