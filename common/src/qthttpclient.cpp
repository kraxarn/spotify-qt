#include "common/qthttpclient.hpp"

cmn::QtHttpClient::QtHttpClient(QObject *parent)
	: QObject(parent),
	lib::http_client()
{
	networkManager = new QNetworkAccessManager(this);
}

auto cmn::QtHttpClient::request(const std::string &url, const lib::headers &headers) ->
QNetworkRequest
{
	// Prepare request
	QNetworkRequest request(QUrl(QString::fromStdString(url)));

	// Set headers
	for (const auto &header : headers)
	{
		request.setRawHeader(QByteArray::fromStdString(header.first),
			QByteArray::fromStdString(header.second));
	}

	// Return prepared request
	return request;
}

void cmn::QtHttpClient::await(QNetworkReply *reply, lib::callback<QByteArray> &callback) const
{
	QNetworkReply::connect(reply, &QNetworkReply::finished, this,
		[reply, callback]()
		{
			callback(reply->readAll());
			reply->deleteLater();
		});
}

void cmn::QtHttpClient::get(const std::string &url, const lib::headers &headers,
	lib::callback<std::string> &callback) const
{
	await(networkManager->get(request(url, headers)),
		[url, callback](const QByteArray &data)
		{
			callback(data.toStdString());
		});
}

void cmn::QtHttpClient::put(const std::string &url, const std::string &body, const lib::headers
&headers,
	lib::callback<std::string> &callback) const
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

void cmn::QtHttpClient::post(const std::string &url, const std::string &body,
	const lib::headers &headers, lib::callback<std::string> &callback) const
{
	auto data = body.empty()
		? QByteArray()
		: QByteArray::fromStdString(body);

	await(networkManager->post(request(url, headers), data),
		[url, callback](const QByteArray &data)
		{
			callback(data.toStdString());
		});
}

auto cmn::QtHttpClient::post(const std::string &url, const lib::headers &headers,
	const std::string &post_data) const -> std::string
{
	// Send request
	auto *reply = networkManager->post(request(url, headers),
		QByteArray::fromStdString(post_data));
	while (!reply->isFinished())
	{
		QCoreApplication::processEvents();
	}

	return reply->readAll().toStdString();
}

void cmn::QtHttpClient::del(const std::string &url, const std::string &body, const lib::headers
&headers,
	lib::callback<std::string> &callback) const
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
