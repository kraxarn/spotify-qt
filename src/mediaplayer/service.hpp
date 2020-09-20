#pragma once

namespace mp
{
	class Service;
}

#include "../spotify/spotify.hpp"
#include "mediaplayer.hpp"
#include "mediaplayerplayer.hpp"

#include <QDBusContext>

namespace mp
{
	class Service: public QObject
	{
	Q_OBJECT

	public:
		Service(spt::Spotify *spotify, QObject *parent);
		virtual ~Service();

		spt::Playback currentPlayback();
		static void signalPropertiesChange(const QObject *adaptor, const QVariantMap &properties);
		void metadataChanged();
		void currentSourceChanged(const spt::Playback &playback);
		void stateUpdated();
		void seekableChanged();
		void volumeChanged();
		void tick(qint64 newPos);
		bool isValid();

	private:
		spt::Spotify *spotify;
		MediaPlayerPlayer *playerPlayer;
	};
}