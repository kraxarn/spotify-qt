#include "lib/qt/httpclient.hpp"
#include "lib/log.hpp"

lib::qt::http_client::http_client(QObject *parent)
	: QObject(parent),
	lib::http_client()
{
	network_manager = new QNetworkAccessManager(this);
}

auto lib::qt::http_client::request(const std::string &url,
	const lib::headers &headers) -> QNetworkRequest
{
	// Prepare request
	QNetworkRequest request(QUrl(QString::fromStdString(url)));

	// Set headers
	for (const auto &header: headers)
	{
		request.setRawHeader(QByteArray::fromStdString(header.first),
			QByteArray::fromStdString(header.second));
	}

	// Return prepared request
	return request;
}

void lib::qt::http_client::await(QNetworkReply *reply, lib::callback<QByteArray> &callback) const
{
	QNetworkReply::connect(reply, &QNetworkReply::finished, this,
		[reply, callback]()
		{
			if (reply->error() != QNetworkReply::NoError)
			{
				lib::log::error("Request failed: {}",
					reply->errorString().toStdString());
			}

			callback(reply->readAll());
			reply->deleteLater();
		});
}

void lib::qt::http_client::await(QNetworkReply *reply,
	lib::callback<lib::result<std::string>> &callback) const
{
	QNetworkReply::connect(reply, &QNetworkReply::finished, this, [reply, callback]()
	{
		const auto data = reply->readAll();
		const std::string response(data.cbegin(), data.cend());

		if (reply->error() != QNetworkReply::NoError)
		{
			const auto statusCode = reply->attribute(
				QNetworkRequest::HttpStatusCodeAttribute
			).toInt();

			if (statusCode > 0 && response.empty())
			{
				std::string statusMessage;
				if (statusCode / 100 == 4)
				{
					statusMessage = lib::fmt::format("Client error: {}", statusCode);
				}
				else if (statusCode / 100 == 5)
				{
					statusMessage = lib::fmt::format("Server error: {}", statusCode);
				}
				else
				{
					statusMessage = lib::fmt::format("Error: {}", statusCode);;
				}

				callback(lib::result<std::string>::fail(statusMessage));
			}
			else
			{
				callback(lib::result<std::string>::fail(response));
			}
		}
		else
		{
			callback(lib::result<std::string>::ok(response));
		}

		reply->deleteLater();
	});
}

void lib::qt::http_client::get(const std::string &url, const lib::headers &headers,
	lib::callback<std::string> &callback) const
{
	await(network_manager->get(request(url, headers)),
		[url, callback](const QByteArray &data)
		{
			callback(data.toStdString());
		});
}

void lib::qt::http_client::get(const std::string &url, const lib::headers &headers,
	lib::callback<lib::result<std::string>> &callback) const
{
	await(network_manager->get(request(url, headers)), callback);
}

void lib::qt::http_client::put(const std::string &url, const std::string &body,
	const lib::headers &headers, lib::callback<std::string> &callback) const
{
	auto data = body.empty()
		? QByteArray()
		: QByteArray::fromStdString(body);

	await(network_manager->put(request(url, headers), data),
		[url, body, callback](const QByteArray &data)
		{
			callback(data.toStdString());
		});
}

void lib::qt::http_client::post(const std::string &url, const std::string &body,
	const lib::headers &headers, lib::callback<std::string> &callback) const
{
	auto data = body.empty()
		? QByteArray()
		: QByteArray::fromStdString(body);

	await(network_manager->post(request(url, headers), data),
		[url, callback](const QByteArray &data)
		{
			callback(data.toStdString());
		});
}

auto lib::qt::http_client::post(const std::string &url, const lib::headers &headers,
	const std::string &post_data) const -> std::string
{
	// Send request
	auto *reply = network_manager->post(request(url, headers),
		QByteArray::fromStdString(post_data));
	while (!reply->isFinished())
	{
		QCoreApplication::processEvents();
	}

	return reply->readAll().toStdString();
}

void lib::qt::http_client::post(const std::string &url, const std::string &body,
	const lib::headers &headers, lib::callback<lib::result<std::string>> &callback) const
{
	const auto data = body.empty()
		? QByteArray()
		: QByteArray::fromStdString(body);

	await(network_manager->post(request(url, headers), data), callback);
}

void lib::qt::http_client::del(const std::string &url, const std::string &body,
	const lib::headers &headers, lib::callback<std::string> &callback) const
{
	auto data = body.empty()
		? QByteArray()
		: QByteArray::fromStdString(body);

	await(network_manager->sendCustomRequest(request(url, headers), "DELETE", data),
		[url, callback](const QByteArray &data)
		{
			callback(data.toStdString());
		});
}
