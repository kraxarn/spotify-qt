#pragma once

namespace mp { class MediaPlayerPlayer; }

#include "../spotify/spotify.hpp"
#include "../mainwindow.hpp"

#include <QDBusConnection>
#include <QDebug>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusAbstractAdaptor>
#include <QCoreApplication>
#include <QVariantMap>

namespace mp
{
	class MediaPlayerPlayer : public QDBusAbstractAdaptor
	{
		Q_OBJECT
		Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")

		Q_PROPERTY(QString PlaybackStatus READ PlaybackStatus NOTIFY playbackStatusChanged)
		Q_PROPERTY(double Rate READ Rate WRITE setRate NOTIFY rateChanged)
		Q_PROPERTY(QVariantMap Metadata READ Metadata NOTIFY playbackStatusChanged)
		Q_PROPERTY(double Volume READ Volume WRITE setVolume NOTIFY volumeChanged)
		Q_PROPERTY(qlonglong Position READ Position WRITE setPropertyPosition NOTIFY playbackStatusChanged)
		Q_PROPERTY(double MinimumRate READ MinimumRate CONSTANT)
		Q_PROPERTY(double MaximumRate READ MaximumRate CONSTANT)
		Q_PROPERTY(bool CanGoNext READ CanGoNext NOTIFY canGoNextChanged)
		Q_PROPERTY(bool CanGoPrevious READ CanGoPrevious NOTIFY canGoPreviousChanged)
		Q_PROPERTY(bool CanPlay READ CanPlay NOTIFY canPlayChanged)
		Q_PROPERTY(bool CanPause READ CanPause NOTIFY canPauseChanged)
		Q_PROPERTY(bool CanSeek READ CanSeek NOTIFY canSeekChanged)
		Q_PROPERTY(int currentTrack READ currentTrack WRITE setCurrentTrack NOTIFY currentTrackChanged)
		Q_PROPERTY(int mediaPlayerPresent READ mediaPlayerPresent WRITE setMediaPlayerPresent NOTIFY mediaPlayerPresentChanged)
		//Q_PROPERTY(bool CanControl READ canControl)
		//Q_PROPERTY(bool Shuffle READ shuffle WRITE setShuffle)

	public:
		explicit MediaPlayerPlayer(spt::Spotify *spotify, QObject *parent = nullptr);
		~MediaPlayerPlayer() override;

		QString PlaybackStatus() const;
		double Rate() const;
		QVariantMap Metadata() const;
		double Volume() const;
		qlonglong Position() const;
		double MinimumRate() const;
		double MaximumRate() const;
		bool CanGoNext() const;
		bool CanGoPrevious() const;
		bool CanPlay() const;
		bool CanPause() const;
		bool CanSeek() const;
		bool CanControl() const;
		int currentTrack() const;
		int mediaPlayerPresent() const;

		bool showProgressOnTaskBar() const;
		void setShowProgressOnTaskBar(bool value);

	signals:
		void Seeked(qlonglong Position);

		void rateChanged(double newRate);
		void volumeChanged(double newVol);
		void playbackStatusChanged();
		void canGoNextChanged();
		void canGoPreviousChanged();
		void canPlayChanged();
		void canPauseChanged();
		void canControlChanged();
		void canSeekChanged();
		void currentTrackChanged();
		void mediaPlayerPresentChanged();
		void next();
		void previous();
		void playPause();
		void stop();

	public slots:
		void emitSeeked(int pos);
		void Next();
		void Previous();
		void Pause();
		void PlayPause();
		void Stop();
		void Play();
		void Seek(qlonglong Offset);
		void SetPosition(const QDBusObjectPath &trackId, qlonglong pos);
		void OpenUri(const QString &uri);

	private slots:
		void playerSourceChanged();
		void playControlEnabledChanged();
		void skipBackwardControlEnabledChanged();
		void skipForwardControlEnabledChanged();
		void playerPlaybackStateChanged();
		void playerIsSeekableChanged();
		void audioPositionChanged();
		void audioDurationChanged();
		void playerVolumeChanged();

	private:
		void signalPropertiesChange(const QString &property, const QVariant &value);

		void setMediaPlayerPresent(int status);
		void setRate(double newRate);
		void setVolume(double volume);
		void setPropertyPosition(int newPositionInMs);
		void setCurrentTrack(int newTrackPosition);

		QVariantMap getMetadataOfCurrentTrack();

		QDBusConnection	dBus;
		spt::Spotify	*spotify;
	};
}