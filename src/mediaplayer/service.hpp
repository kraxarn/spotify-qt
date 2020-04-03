#pragma once

namespace mp { class Service; }

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

		void updateSeeked(qint64 position);

	private:
		spt::Spotify		*spotify;
		MediaPlayer			*player;
		MediaPlayerPlayer	*playerPlayer;
	};
}