#include "mainmenu.hpp"

MainMenu::MainMenu(spt::Spotify &spotify, Settings &settings, QWidget *parent)
	: settings(settings), parent(parent), spotify(spotify), QMenu(parent)
{
	// About
	addAction(Icon::get("help-about"),
		QString("spotify-qt %1").arg(APP_VERSION))->setDisabled(true);
	// Device selection
	auto deviceMenu = new QMenu("Device");
	deviceMenu->setIcon(Icon::get("speaker"));
	QMenu::connect(deviceMenu, &QMenu::aboutToShow, [=]() {
		refreshDevices(deviceMenu);
	});
	addMenu(deviceMenu);
	// Refresh and settings
	auto openSettings = createMenuAction("configure", "Settings...", QKeySequence::Preferences);
	QAction::connect(openSettings, &QAction::triggered, [this]() {
		SettingsDialog dialog(this->settings, this->parent);
		dialog.exec();
	});
	addAction(openSettings);
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
		// Clear client secret/id if clearAll
		if (result == clearAll)
			this->settings.removeClient();
		// Clear login if cleatAll/logOut
		if (result == clearAll || result == logOut)
			this->settings.removeTokens();
		this->settings.save();
		QMessageBox::information(this->parent,
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
	// Probably left menu before it loaded
	if (deviceMenu == nullptr)
		return;
	// Clear all entries
	for (auto &action : deviceMenu->actions())
		deviceMenu->removeAction(action);
	// Check if empty
	if (devices.isEmpty())
	{
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
				window->setStatus(QString("Failed to set device: %1").arg(status), true);
			}
			else
				action->setDisabled(true);
		});
	}
}