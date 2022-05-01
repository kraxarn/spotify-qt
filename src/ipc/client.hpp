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

		void send(const QString &message,
			const std::function<void(const lib::result<QString> &)> &callback);

	private:
		QDataStream in;
		quint32 blockSize;

		auto readResponse() -> QString;
		static auto getErrorMessage(QLocalSocket::LocalSocketError error) -> std::string;
	};
}
