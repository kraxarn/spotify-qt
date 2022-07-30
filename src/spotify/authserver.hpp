#pragma once

#include "lib/qt/spotify/auth.hpp"

#include <QTcpServer>

namespace spt
{
	/**
	 * Spotify authentication server
	 */
	class AuthServer: public QTcpServer
	{
	Q_OBJECT

	public:
		AuthServer(lib::settings &settings, QObject *parent);

		auto listen() -> bool;
		static auto redirectUrl() -> QString;

	signals:
		void success();
		void failed(const QString &message);

	private:
		lib::settings &settings;
		lib::qt::spt::auth auth;

		static constexpr int serverPort = 8888;
		static constexpr int writeTimeout = 3000;

		void onNewConnection();
	};
}
