#pragma once

namespace spt
{
	class Spotify;
}

#include "../dialog/deviceselectdialog.hpp"
#include "../enum/followtype.hpp"
#include "lib/settings.hpp"
#include "artist.hpp"
#include "audiofeatures.hpp"
#include "device.hpp"
#include "playback.hpp"
#include "playlist.hpp"
#include "searchresults.hpp"
#include "user.hpp"

#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcessEnvironment>
#include <QSettings>
#include <QString>
#include <QVector>
#include <QtNetwork>

namespace spt
{
	template<typename T>
	using callback = const std::function<void(const T &)>;

	class Spotify: public QObject
	{
	Q_OBJECT

	public:
		explicit Spotify(lib::settings &settings, QObject *parent = nullptr);

		//region Albums

		QVector<Track> albumTracks(const QString &albumID);

		spt::Album getAlbum(const QString &id);

		QVector<Track> albumTracks(const QString &albumId, const QString &albumName, int offset);

		//endregion

		//region Artists

		void artist(const QString &artistId, callback<spt::Artist> &callback);

		void topTracks(const spt::Artist &artist, callback<std::vector<spt::Track>> &callback);

		void relatedArtists(const spt::Artist &artist,
			callback<std::vector<spt::Artist>> &callback);

		void albums(const spt::Artist &artist, callback<std::vector<spt::Album>> &callback);

		//endregion

		//region Browse

		QVector<Album> newReleases(int offset = 0);

		//endregion

		//region Follow

		QVector<Artist> followedArtists(const QString &offset = QString());

		void follow(FollowType type, const QList<QString> &ids);

		void unfollow(FollowType type, const QList<QString> &ids);

		void isFollowing(FollowType type, const QList<QString> &ids,
			callback<std::vector<bool>> &callback);

		//endregion

		//region Library

		QVector<Album> savedAlbums();

		QVector<Track> savedTracks(int offset = 0);

		QString addSavedTrack(const QString &trackId);

		QString removeSavedTrack(const QString &trackId);

		void isSavedTrack(const QStringList &trackIds,
			callback<std::vector<bool>> &callback);

		//endregion

		//region Personalization

		QVector<Artist> topArtists();

		QVector<Track> topTracks();

		//endregion

		//region Player

		void currentPlayback(callback<spt::Playback> &callback);

		void setDevice(const QString &deviceId, callback<QString> &callback);

		void devices(callback<std::vector<Device>> &callback);

		void playTracks(int trackIndex, const QString &context, callback<QString> &callback);

		void playTracks(int trackIndex, const QList<QString> &all, callback<QString> &callback);

		void playTracks(const QString &context, callback<QString> &callback);

		void resume(callback<QString> &callback);

		void pause(callback<QString> &callback);

		void next(callback<QString> &callback);

		void previous(callback<QString> &callback);

		void seek(int position, callback<QString> &callback);

		void setRepeat(const QString &state, callback<QString> &callback);

		void setVolume(int volume, callback<QString> &callback);

		void setShuffle(bool enabled, callback<QString> &callback);

		QVector<Track> recentlyPlayed();

		void addToQueue(const QString &uri, callback<QString> &callback);

		//endregion

		//region Playlists

		QVector<Playlist> playlists(int offset = 0);

		Playlist playlist(const QString &playlistId);

		QString editPlaylist(const Playlist &playlist);

		QString addToPlaylist(const QString &playlistId, const QString &trackId);

		QString removeFromPlaylist(const QString &playlistId, const QString &trackId, int pos);

		//endregion

		//region Search

		SearchResults search(const QString &query);

		//endregion

		//region Tracks

		spt::Track getTrack(const QString &id);

		AudioFeatures trackAudioFeatures(QString trackId);

		//endregion

		//region User Profile

		void me(callback<User> &callback);

		//endregion

		bool isValid() const;

		/**
		 * @note Calls should not be done directly
		 * @deprecated
		 */
		QJsonObject getAsObject(const QString &url);

	private:
		qint64 lastAuth;
		QString currentDevice;
		lib::settings &settings;
		QNetworkAccessManager *networkManager;
		bool refreshValid = false;

		QNetworkRequest request(const QString &url);

		//region New asynchronous

		void await(QNetworkReply *reply, callback<QByteArray> &callback);

		/**
		 * @note Consider using callback with QJsonObject/QJsonArray instead
		 */
		void get(const QString &url, callback<QJsonDocument> &callback);

		void get(const QString &url, callback<QJsonObject> &callback);

		void get(const QString &url, callback<QJsonArray> &callback);

		void put(const QString &url, const QJsonDocument &body, callback<QString> &callback);

		void put(const QString &url, callback<QString> &callback);

		void post(const QString &url, callback<QString> &callback);

		//endregion

		//region Old synchronous

		/**
		 * @deprecated
		 */
		QString post(const QString &url);

		/**
		 * @note Old synchronous method
		 */
		QString del(const QString &url, const QJsonDocument &json);

		/**
		 * @deprecated
		 */
		QJsonDocument get(const QString &url);

		/**
		 * @deprecated
		 */
		QString put(const QString &url, QVariantMap *body = nullptr);

		//endregion

		/**
		 * @deprecated
		 */
		static QString errorMessage(QNetworkReply *reply);

		/**
		 * @deprecated
		 */
		static QString errorMessage(const QJsonDocument &json, const QUrl &url);

		static QString errorMessage(const QUrl &url, const QByteArray &data);

		/**
		 * @deprecated
		 */
		template<typename T>
		QVector<T> loadItems(const QString &url)
		{
			auto items = getAsObject(url)["items"].toArray();
			QVector<T> result;
			result.reserve(items.count());
			for (auto item : items)
				result.append(T(item.toObject()));
			return result;
		}

		template<typename T>
		void get(const QString &url, callback<std::vector<T>> &callback)
		{
			get(url, [this, callback](const QJsonObject &json)
			{
				auto items = json[json.contains("tracks")
					? "tracks"
					: json.contains("artists")
						? "artists"
						: "items"].toArray();
				std::vector<T> result;
				for (auto item : items)
					result.emplace_back(item.toObject());
				callback(result);
			});
		}

		bool refresh();
		static QString followTypeString(FollowType type);
		static long secondsSinceEpoch();
	};
}