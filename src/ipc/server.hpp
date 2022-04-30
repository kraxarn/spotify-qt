#pragma once

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
		explicit Server(QObject *parent);

		auto start() -> bool;

	private:
		void onNewConnection();
	};
}
