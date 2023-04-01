#include "menu/mainmenubar.hpp"

#include "menu/device.hpp"
#include "menu/developermenu.hpp"
#include "menu/queue.hpp"
#include "dialog/about.hpp"

MainMenuBar::MainMenuBar(lib::spt::api &spotify, lib::settings &settings,
	const lib::http_client &httpClient, lib::cache &cache, QWidget *parent)
	: QMenuBar(parent),
	AppMenu(settings, httpClient, cache, parent)
{
	auto *menu = new QMenu(QStringLiteral(APP_NAME), this);

	auto *preferences = menu->addAction(QStringLiteral("Preferences"));
	preferences->setShortcut(QKeySequence::Preferences);
	QAction::connect(preferences, &QAction::triggered, this, &MainMenuBar::onOpenPreferences);

	auto *about = menu->addAction(QStringLiteral("About"));
	QAction::connect(about, &QAction::triggered, this, &MainMenuBar::onOpenAbout);

	addMenu(menu);

	auto *deviceMenu = new Menu::Device(spotify, this);
	addMenu(deviceMenu);

	auto *queueMenu = new Menu::Queue(spotify, this);
	addMenu(queueMenu);

	auto *accountMenu = new QMenu(QStringLiteral("Account"));

	auto *logOut = accountMenu->addAction(QStringLiteral("Log out"));
	QAction::connect(logOut, &QAction::triggered, this, &MainMenuBar::onLogOut);

	addMenu(accountMenu);

	if (lib::developer_mode::enabled)
	{
		auto *devMenu = new DeveloperMenu(settings, spotify, cache, httpClient, this);
		addMenu(devMenu);
	}
}

void MainMenuBar::onOpenPreferences(bool /*checked*/)
{
	openSettings();
}

void MainMenuBar::onOpenAbout(bool /*checked*/)
{
	auto *dialog = new Dialog::About(this);
	dialog->open();
}

void MainMenuBar::onLogOut(bool /*checked*/)
{
	logOut();
}
