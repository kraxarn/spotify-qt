#pragma once

namespace spt { class Spotify; }

#include "src/settings.hpp"
#include "playlist.hpp"
#include "device.hpp"
#include "playback.hpp"
#include "audiofeatures.hpp"
#include "artist.hpp"
#include "searchresults.hpp"

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
		QString setDevice(const Device &device);
		QString playTracks(const QString &track, const QString &context);
		QString playTracks(const QString &track, const QStringList &all);
		QString playTracks(const QString &context);
		QString setShuffle(bool enabled);
		Playback currentPlayback();
		QString pause();
		QString resume();
		QString seek(int position);
		QString next();
		QString previous();
		QString setVolume(int volume);
		QString setRepeat(const QString &state);
		AudioFeatures trackAudioFeatures(QString trackId);
		QVector<Track> *albumTracks(const QString &albumID);
		Artist artist(const QString &artistId);
		Playlist playlist(const QString &playlistId);
		QString addToPlaylist(const QString &playlistId, const QString &trackId);
		QString removeFromPlaylist(const QString &playlistId, const QString &trackId, int pos);
		SearchResults search(const QString &query);
		QVector<Artist> topArtists();
		QVector<Track> topTracks();
		QVector<Album> savedAlbums();
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
		QString del(QString url, QVariantMap *body);
		static QString errorMessage(QNetworkReply *reply);

		bool refresh();
	};
}