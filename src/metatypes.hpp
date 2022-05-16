#pragma once

#include "lib/spotify/track.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/spotify/device.hpp"
#include "lib/logmessage.hpp"

#include <QMetaType>

Q_DECLARE_METATYPE(lib::log_message)

Q_DECLARE_METATYPE(lib::spt::track)
Q_DECLARE_METATYPE(lib::spt::playlist)
Q_DECLARE_METATYPE(lib::spt::device)
