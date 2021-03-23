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
#include "lib/spotify/playback.hpp"
#include "lib/spotify/playlist.hpp"
#include "searchresults.hpp"
#include "lib/spotify/user.hpp"
#include "thirdparty/json.hpp"
#include "lib/spotify/spotifyapi.hpp"
#include "lib/strings.hpp"

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

		std::vector<lib::spt::track> albumTracks(const std::string &albumId);

		spt::Album getAlbum(const QString &id);

		std::vector<lib::spt::track> albumTracks(const std::string &albumId,
			const std::string &albumName, int offset);

		//endregion

		//region Artists

		void artist(const QString &artistId, lib::callback<spt::Artist> &callback);

		void topTracks(const spt::Artist &artist,
			lib::callback<std::vector<lib::spt::track>> &callback);

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

		std::vector<lib::spt::track> savedTracks(int offset = 0);

		void addSavedTrack(const std::string &trackId, lib::callback<QString> &callback);

		void removeSavedTrack(const std::string &trackId, lib::callback<QString> &callback);

		void isSavedTrack(const QStringList &trackIds,
			lib::callback<std::vector<bool>> &callback);

		//endregion

		//region Personalization

		QVector<Artist> topArtists();

		std::vector<lib::spt::track> topTracks();

		//endregion

		//region Player

		void currentPlayback(lib::callback<lib::spt::playback> &callback);

		void setDevice(const std::string &deviceId, lib::callback<QString> &callback);

		void setDevice(const lib::spt::device &device, lib::callback<QString> &callback);

		void devices(lib::callback<std::vector<lib::spt::device>> &callback);

		void playTracks(int trackIndex, const std::string &context,
			lib::callback<QString> &callback);

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

		std::vector<lib::spt::track> recentlyPlayed();

		void addToQueue(const std::string &uri, lib::callback<std::string> &callback);

		//endregion

		//region Playlists

		std::vector<lib::spt::playlist> playlists(int offset = 0);

		lib::spt::playlist playlist(const std::string &playlistId);

		void editPlaylist(const lib::spt::playlist &playlist,
			lib::callback<std::string> &callback);

		QString addToPlaylist(const std::string &playlistId, const std::string &trackId);

		void removeFromPlaylist(const std::string &playlistId, const std::string &trackId,
			int pos, lib::callback<std::string> &callback);

		//endregion

		//region Search

		SearchResults search(const QString &query);

		//endregion

		//region Tracks

		lib::spt::track getTrack(const std::string &id);

		AudioFeatures trackAudioFeatures(const std::string &trackId);

		//endregion

		//region User Profile

		void me(lib::callback<lib::spt::user> &callback);

		//endregion

		/**
		 * @note Calls should not be done directly
		 * @deprecated
		 */
		QJsonObject getAsObject(const QString &url);

		/**
		 * @note Calls should not be done directly
		 * @deprecated
		 */
		nlohmann::json getAsJson(const QString &url);

	private:
		QNetworkAccessManager *networkManager;

		QNetworkRequest request(const QString &url);

		//region New asynchronous

		void await(QNetworkReply *reply, lib::callback<QByteArray> &callback);

		//region GET

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

		//endregion

		//region PUT

		/**
		 * @deprecated
		 */
		void put(const QString &url, const nlohmann::json &body, lib::callback<QString> &callback);

		/**
		 * @deprecated
		 */
		void put(const QString &url, lib::callback<QString> &callback);

		void put(const std::string &url, const nlohmann::json &body,
			lib::callback<std::string> &callback);

		void put(const std::string &url, lib::callback<std::string> &callback);

		//endregion

		//region POST

		/**
		 * @deprecated
		 */
		void post(const QString &url, lib::callback<QString> &callback);

		void post(const std::string &url, lib::callback<std::string> &callback);

		//endregion

		//region DELETE

		/**
		 * @deprecated
		 */
		void del(const QString &url, const nlohmann::json &json, lib::callback<QString> &callback);

		void del(const std::string &url, const nlohmann::json &json,
			lib::callback<std::string> &callback);

		//endregion

		//region Old synchronous (deprecated)

		/**
		 * @deprecated
		 */
		QString post(const QString &url);

		/**
		 * @deprecated
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

		/**
		 * @deprecated
		 */
		static QString errorMessage(const QUrl &url, const QByteArray &data);

		static std::string error_message(const std::string &url,
			const std::string &data);

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

		/**
		 * @deprecated
		 */
		template<typename T>
		std::vector<T> loadItemsAsJson(const QString &url)
		{
			return getAsJson(url).at("items").get<std::vector<T>>();
		}

		/**
		 * @deprecated
		 */
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

		template<typename T>
		void get(const std::string &url, lib::callback<std::vector<T>> &callback)
		{
			get(url, [this, callback](const nlohmann::json &json)
			{
				auto items = json[json.contains("tracks")
					? "tracks"
					: json.contains("artists")
						? "artists"
						: "items"];
				callback(items.get<std::vector<T>>());
			});
		}

		static QString followTypeString(FollowType type);

		std::string request_refresh(const std::string &post_data,
			const std::string &authorization) override;
	};
}