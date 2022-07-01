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

		auto currentPlayback() -> lib::spt::playback;
		static void signalPropertiesChange(const QObject *adaptor, const QVariantMap &properties);
		void metadataChanged();
		void currentSourceChanged(const lib::spt::playback &playback);
		void stateUpdated();
		void seekableChanged();
		void volumeChanged();
		void seeked(qint64 newPos);
		auto isValid() -> bool;

	private:
		lib::spt::api &spotify;
		MediaPlayerPlayer *playerPlayer = nullptr;
	};
}

#endif
