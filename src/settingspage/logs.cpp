#include "settingspage/logs.hpp"
#include "util/icon.hpp"
#include "view/log/application.hpp"
#include "view/log/spotify.hpp"
#include "mainwindow.hpp"

SettingsPage::Logs::Logs(lib::settings &settings, QWidget *parent)
	: Base(settings, parent)
{
	addTab(appLogs(), "Application logs");
	addTab(spotifyLogs(), "Spotify logs");
}

auto SettingsPage::Logs::icon() -> QIcon
{
	return Icon::get(QStringLiteral("document-preview"));
}

auto SettingsPage::Logs::title() -> QString
{
	return QStringLiteral("Logs");
}

auto SettingsPage::Logs::save() -> bool
{
	return true;
}

auto SettingsPage::Logs::appLogs() -> QWidget *
{
	return new Log::Application(this);
}

auto SettingsPage::Logs::spotifyLogs() -> QWidget *
{
	return new Log::Spotify(this);
}
