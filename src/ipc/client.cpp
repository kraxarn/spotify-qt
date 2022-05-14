#include "client.hpp"
#include "lib/log.hpp"

Ipc::Client::Client(QObject *parent)
	: QLocalSocket(parent)
{
	buffer.setDevice(this);

	QLocalSocket::connect(this, &QLocalSocket::readyRead,
		this, &Ipc::Client::onReadyRead);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
	QLocalSocket::connect(this, &QLocalSocket::errorOccurred,
		this, &Ipc::Client::onErrorOccurred);
#endif
}

void Ipc::Client::send(const QString &message)
{
	blockSize = 0;
	abort();

	connectToServer(APP_NAME);

	QByteArray outData;
	QDataStream out(&outData, QIODevice::WriteOnly);
	out << message.toLocal8Bit();

	write(outData);
	flush();
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

void Ipc::Client::onErrorOccurred(QLocalSocket::LocalSocketError /*error*/)
{
	if (onError)
	{
		onError(errorString());
	}
}
