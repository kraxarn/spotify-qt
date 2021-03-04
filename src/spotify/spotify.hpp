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

		void artist(const QString &artistId,
			const std::function<void(const spt::Artist &artist)> &callback);

		void topTracks(const spt::Artist &artist,
			const std::function<void(const std::vector<spt::Track> &tracks)> &callback);

		void relatedArtists(const spt::Artist &artist,
			const std::function<void(const std::vector<spt::Artist> &artists)> &callback);

		void albums(const spt::Artist &artist,
			const std::function<void(const std::vector<spt::Album> &albums)> &callback);

		//endregion

		//region Browse

		QVector<Album> newReleases(int offset = 0);

		//endregion

		//region Follow

		QVector<Artist> followedArtists(const QString &offset = QString());

		void follow(FollowType type, const QList<QString> &ids);

		void unfollow(FollowType type, const QList<QString> &ids);

		void isFollowing(FollowType type, const QList<QString> &ids,
			const std::function<void(const std::vector<bool> &follows)> &callback);

		//endregion

		//region Library

		QVector<Album> savedAlbums();

		QVector<Track> savedTracks(int offset = 0);

		QString addSavedTrack(const QString &trackId);

		QString removeSavedTrack(const QString &trackId);

		//endregion

		//region Personalization

		QVector<Artist> topArtists();

		QVector<Track> topTracks();

		//endregion

		//region Player

		void currentPlayback(const std::function<void(const spt::Playback &playback)> &callback);

		void setDevice(const QString &deviceId,
			const std::function<void(const QString &status)> &callback);

		void devices(const std::function<void(const std::vector<Device> &devices)> &callback);

		void playTracks(int trackIndex, const QString &context,
			const std::function<void(const QString &result)> &callback);

		void playTracks(int trackIndex, const QList<QString> &all,
			const std::function<void(const QString &result)> &callback);

		void playTracks(const QString &context,
			const std::function<void(const QString &result)> &callback);

		QString resume();

		QString pause();

		QString next();

		QString previous();

		QString seek(int position);

		QString setRepeat(const QString &state);

		QString setVolume(int volume);

		QString setShuffle(bool enabled);

		QVector<Track> recentlyPlayed();

		QString addToQueue(const QString &uri);

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

		void me(const std::function<void(const User &user)> &callback);

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

		void await(QNetworkReply *reply,
			const std::function<void(const QByteArray &response)> &callback);

		void get(const QString &url,
			const std::function<void(const QJsonDocument &json)> &callback);

		void put(const QString &url, const QJsonDocument &body,
			const std::function<void(const QString &result)> &callback);

		void put(const QString &url,
			const std::function<void(const QString &result)> &callback);

		void post(const QString &url,
			const std::function<void(const QString &result)> &callback);

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
		void get(const QString &url,
			const std::function<void(const std::vector<T> &items)> &callback)
		{
			get(url, [this, callback](const QJsonDocument &json)
			{
				auto object = json.object();
				auto items = object[object.contains("tracks")
					? "tracks"
					: object.contains("artists")
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