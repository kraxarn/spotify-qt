#pragma once

namespace spt
{
	class Spotify;
}

#include "../enum/followtype.hpp"
#include "../settings.hpp"
#include "artist.hpp"
#include "audiofeatures.hpp"
#include "device.hpp"
#include "playback.hpp"
#include "playlist.hpp"
#include "searchresults.hpp"
#include "user.hpp"

#include <QCoreApplication>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcessEnvironment>
#include <QSettings>
#include <QString>
#include <QVector>
#include <QtNetwork>

namespace spt
{
	class Spotify: public QObject
	{
	Q_OBJECT

	public:
		explicit Spotify(Settings &settings, QObject *parent = nullptr);

		QJsonObject getAsObject(const QString &url);
		QJsonArray getAsArray(const QString &url);
		QVector<Playlist> playlists(int offset = 0);
		QVector<Device> devices();
		QString setDevice(const Device &device);
		QString playTracks(int trackIndex, const QString &context);
		QString playTracks(int trackIndex, const QList<QString> &all);
		QString playTracks(const QString &context);
		QString setShuffle(bool enabled);
		QString pause();
		QString resume();
		QString seek(int position);
		QString next();
		QString previous();
		QString setVolume(int volume);
		QString setRepeat(const QString &state);
		AudioFeatures trackAudioFeatures(QString trackId);
		QVector<Track> albumTracks(const QString &albumID);
		Artist artist(const QString &artistId);
		Playlist playlist(const QString &playlistId);
		QString addToPlaylist(const QString &playlistId, const QString &trackId);
		QString removeFromPlaylist(const QString &playlistId, const QString &trackId, int pos);
		SearchResults search(const QString &query);
		QVector<Artist> topArtists();
		QVector<Track> topTracks();
		QVector<Album> savedAlbums();
		QVector<Track> savedTracks(int offset = 0);
		QString addSavedTrack(const QString &trackId);
		QString removeSavedTrack(const QString &trackId);
		QVector<Track> recentlyPlayed();
		QVector<Album> newReleases(int offset = 0);
		User me();
		QString editPlaylist(const Playlist &playlist);
		QString addToQueue(const QString &uri);
		QVector<Artist> followedArtists(const QString &offset = QString());
		QVector<bool> isFollowing(FollowType type, const QList<QString> &ids);
		void follow(FollowType type, const QList<QString> &ids);
		void unfollow(FollowType type, const QList<QString> &ids);
		bool isValid() const;

	public slots:
		void requestCurrentPlayback();

	private slots:
		void getLater(const QString &url);

	signals:
		void got(const QJsonDocument &json);
		void gotPlayback(const Playback &playback);

	private:
		qint64 lastAuth;
		QString currentDevice;
		Settings &settings;
		QNetworkAccessManager *networkManager;
		bool refreshValid = false;

		QNetworkRequest request(const QString &url);
		QString put(const QString &url, QVariantMap *body = nullptr);
		QString post(const QString &url);
		QString del(const QString &url, const QJsonDocument &json);
		static QString errorMessage(QNetworkReply *reply);
		static QString errorMessage(const QJsonDocument &json, const QUrl &url);
		QJsonDocument get(const QString &url);

		template<class T>
		QVector<T> loadItems(const QString &url);

		QVector<Track> albumTracks(const QString &albumId, const QString &albumName, int offset);

		bool refresh();
		static QString followTypeString(FollowType type);
	};
}