#include "util/refresher.hpp"
#include "spotify/authserver.hpp"

#define TITLE QStringLiteral("Connection failed")

Refresher::Refresher(lib::settings &settings, spt::Spotify &spotify)
	: settings(settings),
	spotify(spotify)
{
}

void Refresher::refresh(const std::function<void(bool)> &callback)
{
	try
	{
		spotify.refresh();
		callback(true);
	}
	catch (const nlohmann::json::exception &e)
	{
		QMessageBox::warning(nullptr, TITLE,
			QString("Failed to parse response from Spotify:\n%1").arg(e.what()));

		callback(false);
	}
	catch (const lib::spt::error &e)
	{
		QMessageBox::warning(nullptr, TITLE, QString("%1.\nPlease reauthenticate.").arg(e.what()));

		auto *auth = new spt::AuthServer(settings, this);
		if (auth->listen())
		{
			auth->openUrl(nullptr);
		}

		spt::AuthServer::connect(auth, &spt::AuthServer::success, [&callback]()
		{
			callback(true);
		});

		spt::AuthServer::connect(auth, &spt::AuthServer::failed, [&callback](const QString &message)
		{
			QMessageBox::warning(nullptr, TITLE, message);
			callback(false);
		});
	}
	catch (const std::exception &e)
	{
		QMessageBox::warning(nullptr, TITLE,
			QString("Failed to connect to Spotify, check your connection and try again:\n%1")
				.arg(e.what()));

		callback(false);
	}
}
