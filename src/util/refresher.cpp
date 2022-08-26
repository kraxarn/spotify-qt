#include "util/refresher.hpp"
#include "dialog/auth.hpp"

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
		auto *auth = new Dialog::Auth(settings, nullptr);

		Dialog::Auth::connect(auth, &Dialog::Auth::success, [auth, &callback]()
		{
			callback(true);
			auth->deleteLater();
		});

		auth->show(e);
	}
	catch (const std::exception &e)
	{
		QMessageBox::warning(nullptr, TITLE,
			QString("Failed to connect to Spotify, check your connection and try again:\n%1")
				.arg(e.what()));

		callback(false);
	}
}
