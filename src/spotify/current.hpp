#pragma once

#include "lib/spotify/playback.hpp"

#include <QString>

namespace spt
{
	struct Current
	{
		lib::spt::playback playback;
		QString context;
	};
}