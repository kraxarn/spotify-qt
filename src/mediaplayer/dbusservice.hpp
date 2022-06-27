#pragma once

#ifdef USE_DBUS

#include <QString>

#define SERVICE_NAME QStringLiteral("org.mpris.MediaPlayer2.spotify-qt")
#define SERVICE_PATH QStringLiteral("/org/mpris/MediaPlayer2")

#endif
