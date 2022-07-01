#pragma once

#ifdef USE_DBUS

#include "lib/spotify/api.hpp"
#include "util/json.hpp"

#include <QCoreApplication>
#include <QDBusAbstractAdaptor>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QVariantMap>
#include <QJsonObject>

namespace mp
{
	class MediaPlayerPlayer: public QDBusAbstractAdaptor
	{
	Q_OBJECT

		Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")

		Q_PROPERTY(QString PlaybackStatus READ playbackStatus)
		Q_PROPERTY(double Rate READ playbackRate WRITE setPlaybackRate)
		Q_PROPERTY(QVariantMap Metadata READ metadata)
		Q_PROPERTY(double Volume READ getVolume WRITE setVolume)
		Q_PROPERTY(qlonglong Position READ position)
		Q_PROPERTY(double MinimumRate READ playbackRate)
		Q_PROPERTY(double MaximumRate READ playbackRate)
		Q_PROPERTY(bool CanGoNext READ canControl)
		Q_PROPERTY(bool CanGoPrevious READ canControl)
		Q_PROPERTY(bool CanPlay READ canControl)
		Q_PROPERTY(bool CanPause READ canControl)
		Q_PROPERTY(bool CanSeek READ canControl)
		Q_PROPERTY(bool CanControl READ canControl)
		Q_PROPERTY(bool Shuffle READ shuffle WRITE setShuffle)

	public:
		MediaPlayerPlayer(lib::spt::api &spotify, QObject *parent);

		auto canControl() const -> bool;
		auto metadata() const -> QMap<QString, QVariant>;

		auto getVolume() const -> double;
		void setVolume(double value) const;

		auto position() const -> qlonglong;

		auto playbackStatus() const -> QString;

		auto playbackRate() const -> double;
		void setPlaybackRate(double value) const;

		auto shuffle() const -> bool;
		void setShuffle(bool value) const;

		void setCurrentPlayback(const lib::spt::playback &playback);

		void emitMetadataChange() const;
		void currentSourceChanged() const;
		void stateUpdated() const;
		void totalTimeChanged() const;
		void seekableChanged(bool seekable) const;
		void volumeChanged() const;
		void tick(qint64 newPos);

	signals:
		void Seeked(qint64 position);

	public slots:
		void Next() const;
		void Previous() const;
		void Pause() const;
		void PlayPause() const;
		void Stop() const;
		void Play() const;
		void Seek(qint64 offset) const;
		void SetPosition(const QDBusObjectPath &trackId, qint64 position) const;
		void OpenUri(const QString &uri) const;

	private:
		static constexpr qint64 msInUs = 1000;

		QDBusConnection dBus;
		lib::spt::api &spotify;
		std::function<void(const std::string &result)> callback;

		auto currentPlayback() const -> lib::spt::playback;
	};
}

#endif
