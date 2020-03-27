#include "mainmenu.hpp"

MainMenu::MainMenu(spt::Spotify &spotify, QWidget *parent) : QMenu(parent), parent(parent), spotify(spotify)
{
	auto window = (MainWindow*) parent;
	// About
	auto aboutMenu = new QMenu("About");
	auto aboutQt = createMenuAction("logo:qt", "About Qt");
	QAction::connect(aboutQt, &QAction::triggered, [=](bool checked) {
		QMessageBox::aboutQt(this);
	});
	auto checkForUpdates = createMenuAction("download", "Check for updates");
	QAction::connect(checkForUpdates, &QAction::triggered, [window](bool checked) {
		window->setStatus("Checking for updates...");
		auto json = window->getJson("https://api.github.com/repos/kraxarn/spotify-qt/releases");
		auto latest = json.array()[0].toObject()["tag_name"].toString();
		window->setStatus(latest == APP_VERSION
				  ? "You have the latest version"
				  : QString("Update found, latest version is %1, you have version %2")
					  .arg(latest)
					  .arg(APP_VERSION));
	});
	aboutMenu->setIcon(Icon::get("help-about"));
	aboutMenu->addAction(QString("spotify-qt %1").arg(APP_VERSION))->setDisabled(true);
	aboutMenu->addActions({
							  aboutQt, checkForUpdates
						  });
	aboutMenu->addSeparator();
	QAction::connect(
		aboutMenu->addAction(Icon::get("folder-temp"), "Open cache directory"),
		&QAction::triggered, [this, window](bool checked) {
			if (!QDesktopServices::openUrl(QUrl(window->cacheLocation)))
				QMessageBox::warning(this,
					 "No path",
					 QString("Failed to open path: %1").arg(window->cacheLocation));
		}
	);
	QAction::connect(
		aboutMenu->addAction(Icon::get("folder-txt"), "Open config file"),
		&QAction::triggered, [this](bool checked) {
			if (!QDesktopServices::openUrl(QUrl(Settings().fileName())))
				QMessageBox::warning(this,
					 "No file",
					 QString("Failed to open file: %1").arg(Settings().fileName()));
		}
	);
	addMenu(aboutMenu);
	// Device selection
	auto deviceMenu = new QMenu("Device");
	deviceMenu->setIcon(Icon::get("speaker"));
	QMenu::connect(deviceMenu, &QMenu::aboutToShow, [=]() {
		refreshDevices(deviceMenu);
	});
	addMenu(deviceMenu);
	// Refresh and settings
	auto openSettings = createMenuAction("settings", "Settings...", QKeySequence::Preferences);
	QAction::connect(openSettings, &QAction::triggered, [=]() {
		SettingsDialog dialog(this);
		dialog.exec();
	});
	addAction(openSettings);
	// Debug stuff
	addSeparator();
	auto refPlaylist = addAction(
		QIcon::fromTheme("reload"), "Refresh current playlist");
	QAction::connect(refPlaylist, &QAction::triggered, [window]() {
		auto currentPlaylist = window->sptPlaylists->at(window->playlists->currentRow());
		window->refreshPlaylist(currentPlaylist, true);
	});
	// Log out and quit
	addSeparator();
	auto quitAction = createMenuAction("application-exit", "Quit", QKeySequence::Quit);
	QAction::connect(quitAction, &QAction::triggered, QCoreApplication::quit);
	auto logOutAction = createMenuAction("im-user-away",  "Log out");
	QAction::connect(logOutAction, &QAction::triggered, [this](){
		QMessageBox box(
			QMessageBox::Icon::Question,
			"Are you sure?",
			"Do you also want to clear your application credentials or only log out?");
		auto clearAll = box.addButton("Clear everything", QMessageBox::ButtonRole::AcceptRole);
		auto logOut   = box.addButton("Only log out",     QMessageBox::ButtonRole::AcceptRole);
		auto cancel   = box.addButton("Cancel",           QMessageBox::ButtonRole::RejectRole);
		box.exec();
		auto result = box.clickedButton();
		// Return if we pressed cancel
		if (result == nullptr || result == cancel)
			return;
		Settings settings;
		// Clear client secret/id if clearAll
		if (result == clearAll)
			settings.removeClient();
		// Clear login if cleatAll/logOut
		if (result == clearAll || result == logOut)
			settings.removeTokens();
		QMessageBox::information(this,
			 "Logged out",
			 "You are now logged out, the application will now close");
		QCoreApplication::quit();
	});
	addActions({
		 logOutAction, quitAction
	});
}

QAction *MainMenu::createMenuAction(const QString &iconName,
	const QString &text, QKeySequence::StandardKey shortcut)
{
	auto action = new QAction(Icon::get(iconName), text);
	if (shortcut != QKeySequence::UnknownKey)
		action->setShortcut(QKeySequence(shortcut));
	return action;
}

void MainMenu::refreshDevices(QMenu *deviceMenu)
{
	auto window = (MainWindow*) parent;
	// Set status and get devices
	window->setStatus("Refreshing devices...");
	auto devices = spotify.devices();
	// Clear all entries
	for (auto &action : deviceMenu->actions())
		deviceMenu->removeAction(action);
	// Check if empty
	if (devices.isEmpty())
	{
		window->setStatus("No devices found");
		deviceMenu->addAction("No devices found")->setDisabled(true);
		return;
	}
	// Update devices
	window->setStatus(QString("Found %1 device(s)").arg(devices.length()));
	for (auto &device : devices)
	{
		auto action = deviceMenu->addAction(device.name);
		action->setCheckable(true);
		action->setChecked(device.isActive);
		action->setDisabled(device.isActive);
		QAction::connect(action, &QAction::triggered, [=](bool triggered) {
			auto status = spotify.setDevice(device);
			if (!status.isEmpty())
			{
				action->setChecked(false);
				window->setStatus(QString("Failed to set device: %1").arg(status));
			}
			else
				action->setDisabled(true);
		});
	}
}