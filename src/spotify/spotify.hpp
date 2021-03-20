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
#include "lib/spotify/device.hpp"
#include "playback.hpp"
#include "playlist.hpp"
#include "searchresults.hpp"
#include "user.hpp"
#include "thirdparty/json.hpp"
#include "lib/spotify/spotifyapi.hpp"

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
	class Spotify: public QObject, public lib::spt::spotify_api
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

		void artist(const QString &artistId, lib::callback<spt::Artist> &callback);

		void topTracks(const spt::Artist &artist, lib::callback<std::vector<spt::Track>> &callback);

		void relatedArtists(const spt::Artist &artist,
			lib::callback<std::vector<spt::Artist>> &callback);

		void albums(const spt::Artist &artist, lib::callback<std::vector<spt::Album>> &callback);

		//endregion

		//region Browse

		QVector<Album> newReleases(int offset = 0);

		//endregion

		//region Follow

		QVector<Artist> followedArtists(const QString &offset = QString());

		void follow(FollowType type, const QList<QString> &ids);

		void unfollow(FollowType type, const QList<QString> &ids);

		void isFollowing(FollowType type, const QList<QString> &ids,
			lib::callback<std::vector<bool>> &callback);

		//endregion

		//region Library

		QVector<Album> savedAlbums();

		QVector<Track> savedTracks(int offset = 0);

		QString addSavedTrack(const QString &trackId);

		QString removeSavedTrack(const QString &trackId);

		void isSavedTrack(const QStringList &trackIds,
			lib::callback<std::vector<bool>> &callback);

		//endregion

		//region Personalization

		QVector<Artist> topArtists();

		QVector<Track> topTracks();

		//endregion

		//region Player

		void currentPlayback(lib::callback<spt::Playback> &callback);

		void setDevice(const std::string &deviceId, lib::callback<QString> &callback);

		void setDevice(const lib::spt::device &device, lib::callback<QString> &callback);

		void devices(lib::callback<std::vector<lib::spt::device>> &callback);

		void playTracks(int trackIndex, const QString &context, lib::callback<QString> &callback);

		void playTracks(int trackIndex, const std::vector<std::string> &all,
			lib::callback<QString> &callback);

		void playTracks(const QString &context, lib::callback<QString> &callback);

		void resume(lib::callback<QString> &callback);

		void pause(lib::callback<QString> &callback);

		void next(lib::callback<QString> &callback);

		void previous(lib::callback<QString> &callback);

		void seek(int position, lib::callback<QString> &callback);

		void setRepeat(const QString &state, lib::callback<QString> &callback);

		void setVolume(int volume, lib::callback<QString> &callback);

		void setShuffle(bool enabled, lib::callback<QString> &callback);

		QVector<Track> recentlyPlayed();

		void addToQueue(const QString &uri, lib::callback<QString> &callback);

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

		void me(lib::callback<User> &callback);

		//endregion

		/**
		 * @note Calls should not be done directly
		 * @deprecated
		 */
		QJsonObject getAsObject(const QString &url);

	private:
		QNetworkAccessManager *networkManager;

		QNetworkRequest request(const QString &url);

		//region New asynchronous

		void await(QNetworkReply *reply, lib::callback<QByteArray> &callback);

		/**
		 * @deprecated Use callback with json instead
		 */
		void get(const QString &url, lib::callback<QJsonDocument> &callback);

		/**
		 * @deprecated Use callback with json instead
		 */
		void get(const QString &url, lib::callback<QJsonObject> &callback);

		/**
		 * @deprecated Use callback with json instead
		 */
		void get(const QString &url, lib::callback<QJsonArray> &callback);

		void get(const std::string &url, lib::callback<nlohmann::json> &callback);

		void put(const QString &url, const nlohmann::json &body, lib::callback<QString> &callback);

		void put(const QString &url, lib::callback<QString> &callback);

		void post(const QString &url, lib::callback<QString> &callback);

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
		void get(const QString &url, lib::callback<std::vector<T>> &callback)
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

		static QString followTypeString(FollowType type);

		std::string refresh(const std::string &post_data,
			const std::string &authorization) override;
	};
}