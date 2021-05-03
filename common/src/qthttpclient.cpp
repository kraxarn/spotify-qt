#include "common/lib/qthttpclient.hpp"

QtHttpClient::QtHttpClient(lib::settings &settings, QObject *parent)
	: lib::http_client(settings),
	QObject(parent)
{
	networkManager = new QNetworkAccessManager(this);
}

auto QtHttpClient::request(const std::string &url, const lib::headers &headers) -> QNetworkRequest
{
	// Prepare request
	QUrl requestUrl = QString("https://api.spotify.com/v1/%1")
		.arg(QString::fromStdString(url));
	QNetworkRequest request(requestUrl);

	// Set headers
	for (const auto &header : headers)
	{
		request.setRawHeader(QByteArray::fromStdString(header.first),
			QByteArray::fromStdString(header.second));
	}

	// Return prepared request
	return request;
}

void QtHttpClient::await(QNetworkReply *reply, lib::callback<QByteArray> &callback)
{
	QNetworkReply::connect(reply, &QNetworkReply::finished, this,
		[reply, callback]()
		{
			callback(reply->readAll());
			reply->deleteLater();
		});
}

void QtHttpClient::get(const std::string &url, const lib::headers &headers,
	lib::callback<std::string> &callback)
{
	await(networkManager->get(request(url, headers)),
		[url, callback](const QByteArray &data)
		{
			callback(data.toStdString());
		});
}

void QtHttpClient::put(const std::string &url, const std::string &body, const lib::headers &headers,
	lib::callback<std::string> &callback)
{
	auto data = body.empty()
		? QByteArray()
		: QByteArray::fromStdString(body);

	await(networkManager->put(request(url, headers), data),
		[url, body, callback](const QByteArray &data)
		{
			callback(data.toStdString());
		});
}

void QtHttpClient::post(const std::string &url, const lib::headers &headers,
	lib::callback<std::string> &callback)
{
	await(networkManager->post(request(url, headers), QByteArray()),
		[url, callback](const QByteArray &data)
		{
			callback(data.toStdString());
		});
}

void QtHttpClient::del(const std::string &url, const std::string &body, const lib::headers &headers,
	lib::callback<std::string> &callback)
{
	auto data = body.empty()
		? QByteArray()
		: QByteArray::fromStdString(body);

	await(networkManager->sendCustomRequest(request(url, headers), "DELETE", data),
		[url, callback](const QByteArray &data)
		{
			callback(data.toStdString());
		});
}
