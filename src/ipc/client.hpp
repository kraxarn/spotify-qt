#pragma once

#include "lib/result.hpp"

#include <QLocalSocket>
#include <QDataStream>

namespace Ipc
{
	/**
	 * IPC Client
	 */
	class Client: public QLocalSocket
	{
	Q_OBJECT

	public:
		explicit Client(QObject *parent);

		void send(const QString &message);

		void setOnSuccess(const std::function<void(const QString &response)> &callback);
		void setOnError(const std::function<void(const QString &response)> &callback);

	private:
		QDataStream buffer;
		quint32 blockSize;

		std::function<void(const QString &response)> onSuccess;
		std::function<void(const QString &response)> onError;

		auto toString() -> QString;

		void onReadyRead();
		void onErrorOccurred(QLocalSocket::LocalSocketError error);
	};
}
