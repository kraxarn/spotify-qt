#include "mainmenu.hpp"
#include "mainwindow.hpp"
#include "util/menuaction.hpp"
#include "util/shortcut.hpp"
#include "menu/developermenu.hpp"
#include "menu/device.hpp"
#include "menu/queue.hpp"

MainMenu::MainMenu(lib::spt::api &spotify, lib::settings &settings,
	const lib::http_client &httpClient, lib::cache &cache, QWidget *parent)
	: QMenu(parent),
	AppMenu(settings, httpClient, cache, parent)
{
	// Device selection
	auto *deviceMenu = new Menu::Device(spotify, this);
	addMenu(deviceMenu);

	// Track queue
	auto *queueMenu = new Menu::Queue(spotify, this);
	addMenu(queueMenu);

	// Refresh and settings
	auto *openSettings = MenuAction::create("configure", "Settings...",
		this, QKeySequence::Preferences);
	QAction::connect(openSettings, &QAction::triggered,
		this, &MainMenu::onOpenSettings);
	addAction(openSettings);

	// Debug options if enabled
	if (lib::developer_mode::enabled)
	{
		addMenu(new DeveloperMenu(settings, spotify, cache,
			httpClient, this));
	}

	// Log out and quit
	addSeparator();
	auto *quitAction = MenuAction::create("application-exit", "Quit",
		this, QKeySequence::Quit);
	QAction::connect(quitAction, &QAction::triggered, QCoreApplication::quit);

	auto *logOutAction = MenuAction::create(QStringLiteral("im-user-away"),
		QStringLiteral("Log out..."), this, Shortcut::logOut());
	QAction::connect(logOutAction, &QAction::triggered,
		this, &MainMenu::onLogOut);

	addActions({
		logOutAction, quitAction
	});
}

void MainMenu::onLogOut(bool /*checked*/)
{
	logOut();
}

void MainMenu::onOpenSettings(bool /*checked*/)
{
	openSettings();
}
