#pragma once

#include "lib/spotify/track.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/spotify/album.hpp"
#include "lib/spotify/show.hpp"
#include "lib/logmessage.hpp"

#include <QMetaType>

Q_DECLARE_METATYPE(lib::log_message)

Q_DECLARE_METATYPE(lib::spt::track)
Q_DECLARE_METATYPE(lib::spt::playlist)
Q_DECLARE_METATYPE(lib::spt::album)
Q_DECLARE_METATYPE(lib::spt::show)
Q_DECLARE_METATYPE(lib::spt::entity)
