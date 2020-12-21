#pragma once

#include "playback.hpp"

#include <QString>

namespace spt
{
	struct Current
	{
		spt::Playback playback;
		QString context;
	};
}