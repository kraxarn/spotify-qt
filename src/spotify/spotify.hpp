#pragma once

namespace spt
{
	class Spotify;
}

#include "../dialog/deviceselectdialog.hpp"
#include "lib/enum/followtype.hpp"
#include "lib/settings.hpp"
#include "lib/spotify/artist.hpp"
#include "audiofeatures.hpp"
#include "lib/spotify/device.hpp"
#include "lib/spotify/playback.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/spotify/searchresults.hpp"
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

		void getAlbum(const std::string &id, lib::callback<lib::spt::album> &callback);

		std::vector<lib::spt::track> albumTracks(const std::string &albumId,
			const std::string &albumName, int offset);

		//endregion

		//region Artists

		void artist(const std::string &artistId, lib::callback<lib::spt::artist> &callback);

		void topTracks(const lib::spt::artist &artist,
			lib::callback<std::vector<lib::spt::track>> &callback);

		void relatedArtists(const lib::spt::artist &artist,
			lib::callback<std::vector<lib::spt::artist>> &callback);

		void albums(const lib::spt::artist &artist,
			lib::callback<std::vector<lib::spt::album>> &callback);

		//endregion

		//region Browse

		std::vector<lib::spt::album> newReleases(int offset = 0);

		//endregion

		//region Follow

		std::vector<lib::spt::artist> followedArtists(const std::string &offset = std::string());

		void follow(lib::follow_type type, const std::vector<std::string> &ids,
			lib::callback<std::string> &callback);

		void unfollow(lib::follow_type type, const std::vector<std::string> &ids,
			lib::callback<std::string> &callback);

		void isFollowing(lib::follow_type type, const std::vector<std::string> &ids,
			lib::callback<std::vector<bool>> &callback);

		//endregion

		//region Library

		std::vector<lib::spt::album> savedAlbums();

		std::vector<lib::spt::track> savedTracks(int offset = 0);

		void addSavedTrack(const std::string &trackId,
			lib::callback<std::string> &callback);

		void removeSavedTrack(const std::string &trackId,
			lib::callback<std::string> &callback);

		void isSavedTrack(const std::vector<std::string> &trackIds,
			lib::callback<std::vector<bool>> &callback);

		//endregion

		//region Personalization

		std::vector<lib::spt::artist> topArtists();

		std::vector<lib::spt::track> topTracks();

		//endregion

		// Player (moved to lib)

		//region Playlists

		void playlists(lib::callback<std::vector<lib::spt::playlist>> &callback);

		void playlists(int offset, lib::callback<std::vector<lib::spt::playlist>> &callback);

		lib::spt::playlist playlist(const std::string &playlistId);

		void editPlaylist(const lib::spt::playlist &playlist,
			lib::callback<std::string> &callback);

		bool playlistTracks(const lib::spt::playlist &playlist,
			std::vector<lib::spt::track> &trackList);

		std::vector<lib::spt::track> playlistTracks(const lib::spt::playlist &playlist);

		bool playlistTracks(const lib::spt::playlist &playlist,
			std::vector<lib::spt::track> &trackList, const std::string &url, int offset);

		QString addToPlaylist(const std::string &playlistId, const std::string &trackId);

		void removeFromPlaylist(const std::string &playlistId, const std::string &trackId,
			int pos, lib::callback<std::string> &callback);

		//endregion

		//region Search

		void search(const std::string &query, lib::callback<lib::spt::search_results> &callback);

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

		/**
		 * @note Calls should not be done directly
		 * @deprecated
		 */
		nlohmann::json getAsJson(const std::string &url);

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

		void get(const std::string &url, lib::callback<nlohmann::json> &callback) override;

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
			lib::callback<std::string> &callback) override;

		void put(const std::string &url, lib::callback<std::string> &callback) override;

		//endregion

		//region POST

		/**
		 * @deprecated
		 */
		void post(const QString &url, lib::callback<QString> &callback);

		void post(const std::string &url, lib::callback<std::string> &callback) override;

		//endregion

		//region DELETE

		/**
		 * @deprecated
		 */
		void del(const QString &url, const nlohmann::json &json, lib::callback<QString> &callback);

		void del(const std::string &url, const nlohmann::json &json,
			lib::callback<std::string> &callback) override;

		void del(const std::string &url, lib::callback<std::string> &callback) override;

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

		/**
		 * @deprecated
		 */
		static QString followTypeString(lib::follow_type type);

		static std::string follow_type_string(lib::follow_type type);

		std::string request_refresh(const std::string &post_data,
			const std::string &authorization) override;
	};
}