#include "spotify.hpp"

using namespace spt;

Spotify::Spotify(lib::settings &settings, QObject *parent)
	: networkManager(new QNetworkAccessManager(this)),
	lib::spt::api(settings),
	QObject(parent)
{
}

QNetworkRequest Spotify::request(const QString &url)
{
	constexpr int secsInHour = 3600;

	// See when last refresh was
	auto lastRefresh = seconds_since_epoch() - last_auth;
	if (lastRefresh > secsInHour)
	{
		lib::log::info("Access token probably expired, refreshing");
		try
		{
			lib::spt::api::refresh();
		}
		catch (const std::exception &e)
		{
			lib::log::error("Refresh failed: {}", e.what());
		}
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


std::string Spotify::error_message(const std::string &url, const std::string &data)
{
	nlohmann::json json;
	try
	{
		if (!data.empty())
			json = nlohmann::json::parse(data);
	}
	catch (const std::exception &e)
	{
		lib::log::warn("{} failed: {}", url, e.what());
		return std::string();
	}

	if (json.is_null() || !json.is_object() || !json.contains("error"))
		return std::string();

	auto message = json.at("error").at("message").get<std::string>();
	if (!message.empty())
		lib::log::error("{} failed: {}", url, message);
	return message;
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

void Spotify::put(const std::string &url, const nlohmann::json &body,
	lib::callback<std::string> &callback)
{
	// Set in header we're sending json data
	auto req = request(QString::fromStdString(url));
	req.setHeader(QNetworkRequest::ContentTypeHeader,
		QString("application/json"));

	auto data = body.is_null()
		? QByteArray()
		: QByteArray::fromStdString(body.dump());

	await(networkManager->put(req, data),
		[this, url, body, callback](const nlohmann::json &data)
		{
			auto error = error_message(url, data);

			if (lib::strings::contains(error, "No active device found")
				|| lib::strings::contains(error, "Device not found"))
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

void Spotify::post(const std::string &url,
	lib::callback<std::string> &callback)
{
	auto req = request(QString::fromStdString(url));
	req.setHeader(QNetworkRequest::ContentTypeHeader,
		"application/x-www-form-urlencoded");

	await(networkManager->post(req, QByteArray()),
		[url, callback](const nlohmann::json &data)
		{
			callback(error_message(url, data));
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
