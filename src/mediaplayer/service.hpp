#pragma once

namespace mp { class Service; }

#include "mediaplayer.hpp"
#include "mediaplayerplayer.hpp"
#include "properties.hpp"
#include "../spotify/spotify.hpp"

#include <QDBusContext>

namespace mp
{
	class Service : public QObject //, public QDBusContext
	{
	Q_OBJECT

	public:
		Service(spt::Spotify *spotify, QObject *parent);
		virtual ~Service();

		static void signalPropertiesChange(const QObject* adaptor, const QVariantMap& properties);

		void metadataChanged();

		void currentSourceChanged(const spt::Playback &playback);

		void stateUpdated();

		void seekableChanged();

		void volumeChanged();

		void tick(qint64 newPos);

	private:
		spt::Spotify		*spotify;
		MediaPlayer			*player;
		MediaPlayerPlayer	*playerPlayer;
		//Properties			*properties = nullptr;
	};
}