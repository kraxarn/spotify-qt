#include "client.hpp"
#include "lib/log.hpp"

Ipc::Client::Client(QObject *parent)
	: QLocalSocket(parent)
{
	buffer.setDevice(this);

	QLocalSocket::connect(this, &QLocalSocket::readyRead,
		this, &Ipc::Client::onReadyRead);

	QLocalSocket::connect(this, &QLocalSocket::errorOccurred,
		this, &Ipc::Client::onErrorOccurred);
}

void Ipc::Client::send(const QString &message)
{
	blockSize = 0;
	abort();

	connectToServer(APP_NAME);
}

void Ipc::Client::setOnSuccess(const std::function<void(const QString &)> &callback)
{
	onSuccess = callback;
}

void Ipc::Client::setOnError(const std::function<void(const QString &)> &callback)
{
	onError = callback;
}

auto Ipc::Client::toString() -> QString
{
	QString data;
	buffer >> data;
	return data;
}

void Ipc::Client::onReadyRead()
{
	if (blockSize == 0)
	{
		if (bytesAvailable() < static_cast<qint64>(sizeof(quint32)))
		{
			if (onSuccess)
			{
				onSuccess(toString());
			}
			return;
		}
		buffer >> blockSize;
	}

	// Not at the end of stream
	if (bytesAvailable() >= blockSize && !buffer.atEnd())
	{
		return;
	}

	if (onSuccess)
	{
		onSuccess(toString());
	}
}

void Ipc::Client::onErrorOccurred(QLocalSocket::LocalSocketError error)
{
	const auto message = errorString();
	lib::log::error("IPC Client error {}: {}",
		error, message.toStdString());

	if (onError)
	{
		onError(message);
	}
}
