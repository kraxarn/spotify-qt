#include "spotify.hpp"

using namespace spt;

Spotify::Spotify(lib::settings &settings, QObject *parent)
	: networkManager(new QNetworkAccessManager(this)),
	lib::spt::spotify_api(settings),
	QObject(parent)
{
}

QNetworkRequest Spotify::request(const QString &url)
{
	// See when last refresh was
	auto lastRefresh = seconds_since_epoch() - last_auth;
	if (lastRefresh > 3600)
	{
		lib::log::info("Access token probably expired, refreshing");
		lib::spt::spotify_api::refresh();
	}

	// Prepare request
	QNetworkRequest request((QUrl("https://api.spotify.com/v1/" + url)));

	// Set header
	request.setRawHeader("Authorization",
		QString("Bearer %1")
			.arg(QString::fromStdString(settings.account.access_token))
			.toUtf8());

	// Return prepared header
	return request;
}

void Spotify::await(QNetworkReply *reply, lib::callback<QByteArray> &callback)
{
	QNetworkReply::connect(reply, &QNetworkReply::finished, this,
		[reply, callback]()
		{
			callback(reply->readAll());
			reply->deleteLater();
		});
}

QString Spotify::errorMessage(const QUrl &url, const QByteArray &data)
{
	QJsonParseError error{};
	auto json = QJsonDocument::fromJson(data, &error);
	if (error.error != QJsonParseError::NoError)
	{
		// No response, so probably no error
		return QString();
	}

	if (!json.isObject() || !json.object().contains("error"))
		return QString();

	auto message = json.object()["error"].toObject()["message"].toString();
	if (!message.isEmpty())
		lib::log::error("{} failed: {}", url.path().toStdString(), message.toStdString());

	return message;
}

std::string Spotify::error_message(const std::string &url, const std::string &data)
{
	nlohmann::json json;
	try
	{
		json = nlohmann::json::parse(data);
	}
	catch (const std::exception &)
	{
		// No response, so probably no error
		return std::string();
	}

	if (json.is_null() || !json.is_object() || !json.contains("error"))
		return std::string();

	auto message = json.at("error").at("message").get<std::string>();
	if (!message.empty())
		lib::log::error("{} failed: {}", url, message);
	return message;
}

QJsonDocument Spotify::get(const QString &url)
{
	// Send request
	auto reply = networkManager->get(request(url));

	// Wait for request to finish
	while (!reply->isFinished())
		QCoreApplication::processEvents();

	// Parse reply as json
	auto json = QJsonDocument::fromJson(reply->readAll());
	reply->deleteLater();

	// Return parsed json
	errorMessage(json, QUrl(url));
	return json;
}

QJsonObject Spotify::getAsObject(const QString &url)
{
	return get(url).object();
}

nlohmann::json Spotify::getAsJson(const QString &url)
{
	auto response = get(url).toJson().toStdString();
	return response.empty()
		? nlohmann::json()
		: nlohmann::json::parse(response);
}

nlohmann::json Spotify::getAsJson(const std::string &url)
{
	return getAsJson(QString::fromStdString(url));
}

void Spotify::get(const QString &url, lib::callback<QJsonDocument> &callback)
{
	await(networkManager->get(request(url)),
		[callback](const QByteArray &data)
		{
			// Parse reply as json
			callback(QJsonDocument::fromJson(data));
		});
}

void Spotify::get(const QString &url, lib::callback<QJsonObject> &callback)
{
	get(url, [callback](const QJsonDocument &json)
	{
		callback(json.object());
	});
}

void Spotify::get(const QString &url, lib::callback<QJsonArray> &callback)
{
	get(url, [callback](const QJsonDocument &json)
	{
		callback(json.array());
	});
}

void Spotify::get(const std::string &url, lib::callback<nlohmann::json> &callback)
{
	await(networkManager->get(request(QString::fromStdString(url))),
		[url, callback](const QByteArray &data)
		{
			try
			{
				// Parse reply as json
				callback(data.isEmpty()
					? nlohmann::json()
					: nlohmann::json::parse(data.toStdString()));
			}
			catch (const std::exception &e)
			{
				lib::log::error("{} failed: {}", url, e.what());
			}
		});
}

QString Spotify::put(const QString &url, QVariantMap *body)
{
	// Set in header we're sending json data
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

	// Send the request, we don't expect any response
	auto putData = body == nullptr ? nullptr : QJsonDocument::fromVariant(*body).toJson();
	auto reply = errorMessage(networkManager->put(req, putData));
	if (reply.contains("No active device found")
		|| reply.contains("Device not found"))
	{
		devices([this, url, body](const std::vector<lib::spt::device> &devices)
		{
			if (devices.size() == 1)
			{
				this->set_device(devices.at(0),
					[this, url, body](const std::string &status)
					{
						// TODO: This result needs to be handled
						this->put(url, body);
					});
			}
			else if (devices.size() > 1)
			{
				DeviceSelectDialog dialog(devices);
				if (dialog.exec() == QDialog::Accepted)
				{
					auto selected = dialog.selectedDevice();
					if (!selected.id.empty())
					{
						set_device(selected, [this, url, body](const std::string &status)
						{
							// TODO: This result needs to be handled
							this->put(url, body);
						});
					}
				}
			}
		});
	}
	return reply;
}

void Spotify::put(const QString &url, const nlohmann::json &body,
	lib::callback<QString> &callback)
{
	// Set in header we're sending json data
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

	auto data = body.is_null()
		? QByteArray()
		: QByteArray::fromStdString(body.dump());

	await(networkManager->put(req, data),
		[this, url, body, callback](const QByteArray &data)
		{
			auto error = errorMessage(url, data);

			if (error.contains("No active device found"))
			{
				devices([this, url, body, error, callback]
					(const std::vector<lib::spt::device> &devices)
				{
					if (devices.empty())
					{
						if (callback)
							callback(error);
					}
					else if (devices.size() == 1)
					{
						this->set_device(devices.at(0),
							[this, url, body, callback](const std::string &status)
							{
								this->put(url, body, callback);
							});
					}
					else if (devices.size() > 1)
					{
						DeviceSelectDialog dialog(devices);
						if (dialog.exec() == QDialog::Accepted)
						{
							auto selected = dialog.selectedDevice();
							if (!selected.id.empty())
							{
								this->set_device(selected,
									[this, url, body, callback](const std::string &status)
									{
										this->put(url, body, callback);
									});
							}
						}
					}
				});
			}
			else if (callback)
			{
				callback(error);
			}
		});
}

void Spotify::put(const QString &url, lib::callback<QString> &callback)
{
	put(url, nlohmann::json(), callback);
}

void Spotify::put(const std::string &url, const nlohmann::json &body,
	lib::callback<std::string> &callback)
{
	put(QString::fromStdString(url), body, [callback](const QString &result)
	{
		callback(result.toStdString());
	});
}

void Spotify::put(const std::string &url, lib::callback<std::string> &callback)
{
	put(url, nlohmann::json(), callback);
}

void Spotify::post(const QString &url, lib::callback<QString> &callback)
{
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader,
		"application/x-www-form-urlencoded");

	await(networkManager->post(req, QByteArray()),
		[url, callback](const QByteArray &data)
		{
			callback(errorMessage(url, data));
		});
}

void Spotify::post(const std::string &url, lib::callback<std::string> &callback)
{
	post(QString::fromStdString(url), [callback](const QString &result)
	{
		callback(result.toStdString());
	});
}

void Spotify::del(const std::string &url, const nlohmann::json &json,
	lib::callback<std::string> &callback)
{
	auto req = request(QString::fromStdString(url));
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

	auto data = json.is_null()
		? QByteArray()
		: QByteArray::fromStdString(json.dump());

	await(networkManager->sendCustomRequest(req, "DELETE", data),
		[url, callback](const QByteArray &data)
		{
			callback(error_message(url, data.toStdString()));
		});
}

void Spotify::del(const std::string &url, lib::callback<std::string> &callback)
{
	del(url, nlohmann::json(), callback);
}

QString Spotify::post(const QString &url)
{
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	return errorMessage(networkManager->post(req, QByteArray()));
}

QString Spotify::errorMessage(QNetworkReply *reply)
{
	while (!reply->isFinished())
		QCoreApplication::processEvents();
	auto replyBody = reply->readAll();
	reply->deleteLater();
	if (replyBody.isEmpty())
		return QString();

	return errorMessage(QJsonDocument::fromJson(replyBody), reply->url());
}

QString Spotify::errorMessage(const QJsonDocument &json, const QUrl &url)
{
	if (!json.isObject() || !json.object().contains("error"))
		return QString();

	auto message = json.object()["error"].toObject()["message"].toString();
	if (!message.isEmpty())
		lib::log::error("{} failed: {}", url.path().toStdString(), message.toStdString());
	return message;
}

std::string Spotify::request_refresh(const std::string &post_data, const std::string &authorization)
{
	// Create request
	QNetworkRequest request(QUrl("https://accounts.spotify.com/api/token"));
	request.setHeader(QNetworkRequest::ContentTypeHeader,
		"application/x-www-form-urlencoded");
	request.setRawHeader("Authorization",
		QString::fromStdString(authorization).toUtf8());

	// Send request
	auto reply = networkManager->post(request, QByteArray::fromStdString(post_data));
	while (!reply->isFinished())
		QCoreApplication::processEvents();

	return reply->readAll().toStdString();
}
