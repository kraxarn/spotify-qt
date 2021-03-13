#include "spotify.hpp"

using namespace spt;

Spotify::Spotify(lib::settings &settings, QObject *parent)
	: settings(settings),
	QObject(parent)
{
	lastAuth = 0;
	networkManager = new QNetworkAccessManager(this);

	if (secondsSinceEpoch() - settings.account.last_refresh < 3600)
	{
		lib::log::info("Last refresh was less than an hour ago, not refreshing access token");
		lastAuth = settings.account.last_refresh;
		refreshValid = true;
		return;
	}

	refreshValid = refresh();
}

QNetworkRequest Spotify::request(const QString &url)
{
	// See when last refresh was
	auto lastRefresh = secondsSinceEpoch() - lastAuth;
	if (lastRefresh >= 3600)
	{
		lib::log::info("Access token probably expired, refreshing");
		refresh();
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

void Spotify::await(QNetworkReply *reply, callback<QByteArray> &callback)
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

void Spotify::get(const QString &url, callback<QJsonDocument> &callback)
{
	await(networkManager->get(request(url)),
		[callback](const QByteArray &data)
		{
			// Parse reply as json
			callback(QJsonDocument::fromJson(data));
		});
}

void Spotify::get(const QString &url, callback<QJsonObject> &callback)
{
	get(url, [callback](const QJsonDocument &json)
	{
		callback(json.object());
	});
}

void Spotify::get(const QString &url, callback<QJsonArray> &callback)
{
	get(url, [callback](const QJsonDocument &json)
	{
		callback(json.array());
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
	if (reply.contains("No active device found"))
	{
		devices([this, url, body](const std::vector<spt::Device> &devices)
		{
			if (devices.size() == 1)
			{
				this->setDevice(devices.at(0).id,
					[this, url, body](const QString &status)
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
					if (!selected.id.isEmpty())
					{
						setDevice(selected.id, [this, url, body](const QString &status)
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

void Spotify::put(const QString &url, const QJsonDocument &body, callback<QString> &callback)
{
	// Set in header we're sending json data
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));

	await(networkManager->put(req, body.toJson()),
		[this, url, body, callback](const QByteArray &data)
		{
			auto error = errorMessage(url, data);

			if (error.contains("No active device found"))
			{
				devices([this, url, body, callback](const std::vector<spt::Device> &devices)
				{
					if (devices.size() == 1)
					{
						this->setDevice(devices.at(0).id,
							[this, url, body, callback](const QString &status)
							{
								this->put(url, body, callback);
							});
						return;
					}
					else if (devices.size() > 1)
					{
						DeviceSelectDialog dialog(devices);
						if (dialog.exec() == QDialog::Accepted)
						{
							auto selected = dialog.selectedDevice();
							if (!selected.id.isEmpty())
							{
								this->setDevice(selected.id,
									[this, url, body, callback](const QString &status)
									{
										this->put(url, body, callback);
									});
								return;
							}
						}
					}
				});
			}

			callback(error);
		});
}

void Spotify::put(const QString &url, callback<QString> &callback)
{
	put(url, QJsonDocument(), callback);
}

void Spotify::post(const QString &url, callback<QString> &callback)
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

QString Spotify::post(const QString &url)
{
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	return errorMessage(networkManager->post(req, QByteArray()));
}

QString Spotify::del(const QString &url, const QJsonDocument &json)
{
	auto req = request(url);
	req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	return errorMessage(networkManager->sendCustomRequest(req, "DELETE", json.toJson()));
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

bool Spotify::refresh()
{
	// Make sure we have a refresh token
	auto refreshToken = settings.account.refresh_token;
	if (refreshToken.empty())
	{
		lib::log::warn("Attempt to refresh without refresh token");
		return false;
	}

	// Create form
	auto postData = QString("grant_type=refresh_token&refresh_token=%1")
		.arg(QString::fromStdString(refreshToken))
		.toUtf8();

	// Create request
	QNetworkRequest request(QUrl("https://accounts.spotify.com/api/token"));
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	request.setRawHeader("Authorization", "Basic " + QString("%1:%2")
		.arg(QString::fromStdString(settings.account.client_id))
		.arg(QString::fromStdString(settings.account.client_secret))
		.toUtf8().toBase64());

	// Send request
	auto reply = networkManager->post(request, postData);
	while (!reply->isFinished())
		QCoreApplication::processEvents();

	// Parse json
	auto json = QJsonDocument::fromJson(reply->readAll()).object();
	reply->deleteLater();

	// Check if error
	if (json.contains("error_description") || !json.contains("access_token"))
	{
		auto error = json["error_description"].toString();
		lib::log::warn("Failed to refresh token: {}", error.isEmpty()
			? "no access token"
			: error.toStdString());
		return false;
	}

	// Save as access token
	lastAuth = secondsSinceEpoch();
	auto accessToken = json["access_token"].toString().toStdString();
	settings.account.last_refresh = lastAuth;
	settings.account.access_token = accessToken;
	settings.save();
	return true;
}

QString Spotify::followTypeString(FollowType type)
{
	switch (type)
	{
		case FollowType::Artist:
			return "artist";

		case FollowType::User:
			return "user";
	}

	return QString();
}

bool Spotify::isValid() const
{
	return refreshValid;
}

long Spotify::secondsSinceEpoch()
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()
		.time_since_epoch()).count();
}
