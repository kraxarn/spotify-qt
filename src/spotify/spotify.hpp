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
#include <QObject>
#include <QRandomGenerator>

namespace spt
{
	class Spotify : public QObject
	{
		Q_OBJECT

	public:
		Spotify();
		~Spotify();
		/**
		 * HTTP GET request expecting JSON response
		 */
		quint32 get(const QString &url);
		void getPlaylists();
		QVector<Device> devices();
		QString setDevice(Device device);
		QString playTracks(const QString &track, const QString &context);
		QString playTracks(const QString &track, const QStringList &all);
		QString playTracks(const QString &context);
		QString setShuffle(bool enabled);
		void getCurrentPlayback();
		QString pause();
		QString resume();
		QString seek(int position);
		QString next();
		QString previous();
		QString setVolume(int volume);
		QString setRepeat(QString state);
		void getTrackAudioFeatures(QString trackId);
		void getAlbumTracks(const QString &albumID);
		void getArtist(const QString &artistId);
		void getPlaylist(const QString &playlistId);
		QString addToPlaylist(const QString &playlistId, const QString &trackId);
		QString removeFromPlaylist(const QString &playlistId, const QString &trackId, int pos);

	signals:
		void error(const QString &message);
		void jsonResponse(quint32 id, const QJsonDocument &json);
		void playlistsResponse(const QVector<Playlist> &playlists);
		void devicesResponse(const QVector<Device> &devices);
		void currentPlaybackResponse(const Playback &playback);
		void audioFeaturesResponse(const AudioFeatures &features);
		void albumTracksResponse(const QVector<Track> &tracks);
		void artistResponse(const Artist &artist);
		void playlistResponse(Playlist &playlist);

	private:
		qint64					lastAuth;
		QString					currentDevice;
		QNetworkAccessManager	*networkManager;

		/**
		 * Prepare network request with auth header
		 */
		QNetworkRequest request(const QString &url);
		/**
		 * HTTP PUT request expecting JSON response
		 */
		QString put(QString url, QVariantMap *body = nullptr);
		QString post(QString url);
		QString del(QString url, QVariantMap *body);
		static QString errorMessage(QNetworkReply *reply);

		bool refresh();
		static quint32 randomId();
	};
}