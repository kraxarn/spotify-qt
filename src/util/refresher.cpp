#include "util/refresher.hpp"
#include "dialog/setup.hpp"
#include "lib/spotify/error.hpp"

#include <QMessageBox>

#define TITLE QStringLiteral("Connection failed")

Refresher::Refresher(lib::settings &settings, lib::spt::request &spotify)
	: settings(settings),
	spotify(spotify)
{
}

auto Refresher::refresh() -> bool
{
	try
	{
		spotify.refresh();
		return true;
	}
	catch (const nlohmann::json::exception &e)
	{
		QMessageBox::warning(nullptr, TITLE,
			QStringLiteral("Failed to parse response from Spotify:\n%1").arg(e.what()));

		return false;
	}
	catch (const lib::spt::error &e)
	{
		lib::log::error("Failed to refresh token: {}", e.what());

		Dialog::Setup dialog(settings, nullptr);
		return dialog.exec() == QDialog::Accepted;
	}
	catch (const std::exception &e)
	{
		QMessageBox::warning(nullptr, TITLE,
			QStringLiteral("Failed to connect to Spotify, check your connection and try again:\n%1")
				.arg(e.what()));

		return false;
	}
}
