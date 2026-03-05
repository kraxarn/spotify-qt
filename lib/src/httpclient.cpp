#include "lib/httpclient.hpp"
#include "lib/log.hpp"

#include <QNetworkReply>

HttpClient::HttpClient(QObject *parent)
	: QObject(parent),
	mNetworkManager(new QNetworkAccessManager(this))
{
}

void HttpClient::get(const std::string &url, const lib::headers &headers,
	ApiCallback<std::string> &callback) const
{
	await(mNetworkManager->get(request(url, headers)),
		[callback](const QByteArray &data) -> void
		{
			callback(data.toStdString());
		});
}

void HttpClient::get(const std::string &url, const lib::headers &headers,
	ApiCallback<Result<std::string>> &callback) const
{
	await(mNetworkManager->get(request(url, headers)), callback);
}

void HttpClient::get(const QUrl &url, const RequestHeaders &headers,
	ApiCallback<Result<QByteArray>> &callback) const
{
	await(mNetworkManager->get(request(url, headers)), callback);
}

void HttpClient::put(const std::string &url, const std::string &body,
	const lib::headers &headers, ApiCallback<std::string> &callback) const
{
	const QByteArray data = body.empty()
		? QByteArray()
		: QByteArray::fromStdString(body);

	await(mNetworkManager->put(request(url, headers), data),
		[callback](const QByteArray &data) -> void
		{
			callback(data.toStdString());
		});
}

void HttpClient::post(const std::string &url, const lib::headers &headers,
	ApiCallback<std::string> &callback) const
{
	post(url, std::string(), headers, callback);
}

void HttpClient::post(const std::string &url, const lib::headers &headers,
	ApiCallback<Result<std::string>> &callback) const
{
	post(url, {}, headers, callback);
}

void HttpClient::post(const std::string &url, const std::string &body,
	const lib::headers &headers, ApiCallback<std::string> &callback) const
{
	const QByteArray data = body.empty()
		? QByteArray()
		: QByteArray::fromStdString(body);

	await(mNetworkManager->post(request(url, headers), data),
		[callback](const QByteArray &data) -> void
		{
			callback(data.toStdString());
		});
}

auto HttpClient::post(const std::string &url, const lib::headers &headers,
	const std::string &post_data) const -> std::string
{
	// Send request
	auto *reply = mNetworkManager->post(request(url, headers),
		QByteArray::fromStdString(post_data));

	while (!reply->isFinished())
	{
		QCoreApplication::processEvents();
	}

	return reply->readAll().toStdString();
}

void HttpClient::post(const QUrl &url, const QByteArray &body, const RequestHeaders &headers,
	ApiCallback<Result<QByteArray>> &callback) const
{
	await(mNetworkManager->post(request(url, headers), body), callback);
}

void HttpClient::post(const std::string &url, const std::string &body,
	const lib::headers &headers, ApiCallback<Result<std::string>> &callback) const
{
	const QByteArray data = body.empty()
		? QByteArray()
		: QByteArray::fromStdString(body);

	await(mNetworkManager->post(request(url, headers), data), callback);
}

void HttpClient::del(const std::string &url, const std::string &body,
	const lib::headers &headers, ApiCallback<std::string> &callback) const
{
	auto data = body.empty()
		? QByteArray()
		: QByteArray::fromStdString(body);

	await(mNetworkManager->sendCustomRequest(request(url, headers), "DELETE", data),
		[url, callback](const QByteArray &data)
		{
			callback(data.toStdString());
		});
}

void HttpClient::deleteResource(const QUrl &url, const QByteArray &body,
	const RequestHeaders &headers, ApiCallback<Result<QByteArray>> &callback) const
{
	await(mNetworkManager->sendCustomRequest(request(url, headers),
		"DELETE", body), callback);
}

auto HttpClient::request(const std::string &url,
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

auto HttpClient::request(const QUrl &url, const RequestHeaders &headers) -> QNetworkRequest
{
	QNetworkRequest request(url);

	QMapIterator iter(headers);
	while (iter.hasNext())
	{
		iter.next();
		request.setRawHeader(iter.key().toUtf8(), iter.value().toUtf8());
	}

	return request;
}

void HttpClient::await(QNetworkReply *reply, ApiCallback<QByteArray> &callback) const
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

void HttpClient::await(QNetworkReply *reply, ApiCallback<Result<std::string>> &callback) const
{
	await(reply, [callback](const Result<QByteArray> &result) -> void
	{
		if (!result.success())
		{
			callback(Result<std::string>::fail(result.message()));
			return;
		}

		callback(Result<std::string>::ok(result.value().toStdString()));
	});
}

void HttpClient::await(QNetworkReply *reply, ApiCallback<Result<QByteArray>> &callback) const
{
	connect(reply, &QNetworkReply::finished, this, [reply, callback]()
	{
		const QByteArray response = reply->readAll();

		if (reply->error() != QNetworkReply::NoError)
		{
			const auto statusCode = reply->attribute(
				QNetworkRequest::HttpStatusCodeAttribute
			).toInt();

			if (statusCode > 0 && response.isEmpty())
			{
				QString statusMessage;
				if (statusCode / 100 == 4)
				{
					statusMessage = QStringLiteral("Client error: %1").arg(statusCode);
				}
				else if (statusCode / 100 == 5)
				{
					statusMessage = QStringLiteral("Server error: %1").arg(statusCode);
				}
				else
				{
					statusMessage = QStringLiteral("Error: %1").arg(statusCode);
				}

				callback(Result<QByteArray>::fail(statusMessage));
			}
			else
			{
				callback(Result<QByteArray>::fail(QString::fromUtf8(response)));
			}
		}
		else
		{
			callback(Result<QByteArray>::ok(response));
		}

		reply->deleteLater();
	});
}
