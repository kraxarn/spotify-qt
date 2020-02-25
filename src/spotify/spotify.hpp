#pragma once

namespace spt { class Spotify; }

#include "src/settings.hpp"
#include "playlist.hpp"
#include "device.hpp"
#include "playback.hpp"
#include "audiofeatures.hpp"
#include "artist.hpp"

#include <QDateTime>
#include <QtNetwork>
#include <QSettings>
#include <QProcessEnvironment>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QDesktopServices>
#include <QInputDialog>
#include <QCoreApplication>

namespace spt
{
	class Spotify
	{
	public:
		Spotify();
		~Spotify();
		/**
		 * HTTP GET request expecting JSON response
		 */
		QJsonDocument get(QString url);
		void playlists(QVector<Playlist> **playlists);
		QVector<Device> devices();
		QString setDevice(Device device);
		QString playTracks(const QString &track, const QString &context);
		QString playTracks(const QString &track, const QStringList &all);
		QString setShuffle(bool enabled);
		Playback currentPlayback();
		QString pause();
		QString resume();
		QString seek(int position);
		QString next();
		QString previous();
		QString setVolume(int volume);
		QString setRepeat(QString state);
		AudioFeatures trackAudioFeatures(QString trackId);
		QVector<Track> *albumTracks(const QString &albumID);
		Artist artist(const QString &artistId);
		QVector<Track> artistTopTracks(const QString &artistId);
	private:
		qint64		lastAuth;
		QString		currentDevice;
		QNetworkAccessManager *networkManager;
		/**
		 * Prepare network request with auth header
		 */
		QNetworkRequest request(QString &url);
		/**
		 * HTTP PUT request expecting JSON response
		 */
		QString put(QString url, QVariantMap *body = nullptr);
		QString post(QString url);
		static QString errorMessage(QNetworkReply *reply);

		bool refresh();
	};
}