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

		//region Browse

		std::vector<lib::spt::album> newReleases(int offset = 0);

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

		void get(const std::string &url, lib::callback<nlohmann::json> &callback) override;

		void put(const std::string &url, const nlohmann::json &body,
			lib::callback<std::string> &callback) override;

		void post(const std::string &url, lib::callback<std::string> &callback) override;

		void del(const std::string &url, const nlohmann::json &json,
			lib::callback<std::string> &callback) override;

		//endregion

		//region Old synchronous (deprecated)

		/**
		 * @deprecated
		 */
		QString post(const QString &url);

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

		std::string request_refresh(const std::string &post_data,
			const std::string &authorization) override;
	};
}