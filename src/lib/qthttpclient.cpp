#include "qthttpclient.hpp"

QtHttpClient::QtHttpClient(lib::settings &settings, QObject *parent)
	: networkManager(new QNetworkAccessManager(this)),
	lib::http_client(settings),
	QObject(parent)
{
}

auto QtHttpClient::request(const std::string &url) -> QNetworkRequest
{
	// Prepare request
	QUrl requestUrl = QString("https://api.spotify.com/v1/%1")
		.arg(QString::fromStdString(url));
	QNetworkRequest request(requestUrl);

	// Set header
	request.setRawHeader("Authorization",
		QByteArray::fromStdString(authorization_header()));

	// Return prepared header
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

void QtHttpClient::get(const std::string &url,
	lib::callback<std::string> &callback)
{
	await(networkManager->get(request(url)), [url, callback](const QByteArray &data)
	{
		callback(data.toStdString());
	});
}

void QtHttpClient::put(const std::string &url, const std::string &body,
	lib::callback<std::string> &callback)
{
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader,
		QString("application/json"));

	auto data = body.empty()
		? QByteArray()
		: QByteArray::fromStdString(body);

	await(networkManager->put(req, data), [url, body, callback](const QByteArray &data)
	{
		callback(data.toStdString());
	});
}

void QtHttpClient::post(const std::string &url,
	lib::callback<std::string> &callback)
{
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader,
		"application/x-www-form-urlencoded");

	await(networkManager->post(req, QByteArray()),
		[url, callback](const QByteArray &data)
		{
			callback(data.toStdString());
		});
}

void QtHttpClient::del(const std::string &url, const std::string &body,
	lib::callback<std::string> &callback)
{
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	auto data = body.empty()
		? QByteArray()
		: QByteArray::fromStdString(body);

	await(networkManager->sendCustomRequest(req, "DELETE", data),
		[url, callback](const QByteArray &data)
		{
			callback(data.toStdString());
		});
}

auto QtHttpClient::post(const std::string &data, const std::string &authorization) -> std::string
{
	// Create request
	QNetworkRequest request(QUrl("https://accounts.spotify.com/api/token"));
	request.setHeader(QNetworkRequest::ContentTypeHeader,
		"application/x-www-form-urlencoded");
	request.setRawHeader("Authorization",
		QByteArray::fromStdString(authorization));

	// Send request
	auto *reply = networkManager->post(request, QByteArray::fromStdString(data));
	while (!reply->isFinished())
	{
		QCoreApplication::processEvents();
	}

	return reply->readAll().toStdString();
}
