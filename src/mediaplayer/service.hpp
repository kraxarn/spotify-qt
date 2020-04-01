#pragma once

#include "mediaplayer.hpp"
#include "mediaplayerplayer.hpp"
#include "../spotify/spotify.hpp"

#include <QDBusContext>

namespace mp
{
	class Service : public QObject, public QDBusContext
	{
	Q_OBJECT

	public:
		Service(spt::Spotify *spotify, QObject *parent);
		virtual ~Service();

	private:
		MediaPlayer			*player;
		MediaPlayerPlayer	*playerPlayer;
	};
}