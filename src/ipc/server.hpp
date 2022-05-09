#pragma once

#include "lib/spotify/api.hpp"

#include <QtNetwork>

namespace Ipc
{
	/**
	 * IPC Server
	 */
	class Server: public QLocalServer
	{
	Q_OBJECT

	public:
		Server(lib::spt::api &spotify, QObject *parent);

		auto start() -> bool;

	private:
		QLocalSocket *socket = nullptr;
		QDataStream stream;
		lib::spt::api &spotify;

		void onNewConnection();
		void onReadyRead();
		auto onReadAll(const QString &data) -> bool;
		static void logStatus(const QString &data, const std::string &status);
	};
}
