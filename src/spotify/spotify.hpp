#pragma once

namespace spt
{
	class Spotify;
}

#include "../dialog/deviceselectdialog.hpp"
#include "lib/enum/followtype.hpp"
#include "lib/settings.hpp"
#include "lib/spotify/artist.hpp"
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
#include <QMessageBox>

namespace spt
{
	class Spotify: public QObject, public lib::spt::api
	{
	Q_OBJECT

	public:
		explicit Spotify(lib::settings &settings, QObject *parent = nullptr);

		auto tryRefresh() -> bool;

	private:
		QNetworkAccessManager *networkManager;

		auto request(const QString &url) -> QNetworkRequest;

		void select_device(const std::vector<lib::spt::device> &devices,
			lib::callback<lib::spt::device> &callback);

		//region New asynchronous

		void await(QNetworkReply *reply, lib::callback<QByteArray> &callback);

		void get(const std::string &url, lib::callback<nlohmann::json> &callback) override;

		void put(const std::string &url, const nlohmann::json &body,
			lib::callback<std::string> &callback) override;

		void post(const std::string &url, lib::callback<std::string> &callback) override;

		void del(const std::string &url, const nlohmann::json &json,
			lib::callback<std::string> &callback) override;

		//endregion

		static auto error_message(const std::string &url,
			const std::string &data) -> std::string;

		auto request_refresh(const std::string &post_data,
			const std::string &authorization) -> std::string override;
	};
}
