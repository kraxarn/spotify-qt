#include "client.hpp"
#include "lib/log.hpp"

Ipc::Client::Client(QObject *parent)
	: QLocalSocket(parent)
{
	in.setDevice(this);
}

void Ipc::Client::send(const QString &message,
	const std::function<void(const lib::result<QString> &)> &callback)
{
	blockSize = 0;
	abort();

	QLocalSocket::connect(this, &QLocalSocket::readyRead,
		[this, callback]()
		{
			const auto response = Ipc::Client::readResponse(this);
			callback(lib::result<QString>::ok(response));
		});

	QLocalSocket::connect(this, &QLocalSocket::errorOccurred,
		[callback](QLocalSocket::LocalSocketError error)
		{
			const auto message = Ipc::Client::getErrorMessage(error);
			callback(lib::result<QString>::fail(message));
		});

	connectToServer(APP_NAME);
}

auto Ipc::Client::readResponse() -> QString
{
	quint32 blockSize;
	QDataStream buffer;

	if (socket->bytesAvailable() < static_cast<qint64>(sizeof(quint32)))
	{
		lib::log::warn("Socket buffer full");
		return {};
	}
	buffer >> blockSize;

	if (socket->bytesAvailable() < blockSize || buffer.atEnd())
	{
		lib::log::warn("No data in socket");
		return {};
	}

	QString response;
	buffer >> response;
	return response;
}

auto Ipc::Client::getErrorMessage(QLocalSocket::LocalSocketError error) -> std::string
{
	switch (error)
	{
		case ConnectionRefusedError:
			return {"connection refused"};

		case PeerClosedError:
			return {"connection closed"};

		case ServerNotFoundError:
			return {"server not found"};

		case SocketAccessError:
			return {"access denied"};

		case SocketResourceError:
			return {"too many active connections"};

		case SocketTimeoutError:
			return {"timed out"};

		case DatagramTooLargeError:
			return {"too much data"};

		case ConnectionError:
			return {"connection error"};

		case UnsupportedSocketOperationError:
			return {"not supported"};

		case OperationError:
			return {"there is already an active operation"};

		case UnknownSocketError:
			return {"unknown error"};

		default:
			return {};
	}
}

