#pragma once

#ifdef USE_DBUS

#include "lib/spotify/api.hpp"
#include "mediaplayer.hpp"
#include "mediaplayerplayer.hpp"

#include <QDBusContext>
#include <QMetaClassInfo>

namespace mp
{
	class Service: public QObject
	{
	Q_OBJECT

	public:
		Service(lib::spt::api &spotify, QObject *parent);

		lib::spt::playback currentPlayback();
		static void signalPropertiesChange(const QObject *adaptor, const QVariantMap &properties);
		void metadataChanged();
		void currentSourceChanged(const lib::spt::playback &playback);
		void stateUpdated();
		void seekableChanged();
		void volumeChanged();
		void tick(qint64 newPos);
		bool isValid();

	private:
		lib::spt::api &spotify;
		MediaPlayerPlayer *playerPlayer = nullptr;
	};
}

#endif
