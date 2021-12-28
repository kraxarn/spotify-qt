#include "mainmenu.hpp"
#include "mainwindow.hpp"

MainMenu::MainMenu(lib::spt::api &spotify, lib::settings &settings,
	const lib::http_client &httpClient, lib::cache &cache, QWidget *parent)
	: QMenu(parent),
	spotify(spotify),
	settings(settings),
	cache(cache),
	httpClient(httpClient)
{
	// Update notifier
	about = addAction(Icon::get("help-about"), QString("Checking for updates..."));
	about->setDisabled(true);

	// Check for updates
	httpClient.get("https://api.github.com/repos/kraxarn/spotify-qt/releases/latest",
		lib::headers(), [this](const std::string &data)
		{
			this->checkForUpdate(data);
		});

	// Device selection
	deviceMenu = new QMenu("Device", this);
	deviceMenu->setIcon(Icon::get("speaker"));
	QMenu::connect(deviceMenu, &QMenu::aboutToShow, [this]()
	{
		this->refreshDevices();
	});
	QMenu::connect(deviceMenu, &QMenu::triggered, this, &MainMenu::deviceSelected);
	addMenu(deviceMenu);

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

	auto *logOutAction = MenuAction::create("im-user-away", "Log out", this);
	QAction::connect(logOutAction, &QAction::triggered,
		this, &MainMenu::logOut);

	addActions({
		logOutAction, quitAction
	});
}

void MainMenu::refreshDevices()
{
	spotify.devices([this](const std::vector<lib::spt::device> &devices)
	{
		// Probably left menu before it loaded
		if (this->deviceMenu == nullptr)
		{
			return;
		}

		// Clear all entries
		for (auto &action: deviceMenu->actions())
		{
			deviceMenu->removeAction(action);
		}

		// Check if empty
		if (devices.empty())
		{
			deviceMenu->addAction("No devices found")->setDisabled(true);
			return;
		}

		// Update devices
		for (const auto &device: devices)
		{
			auto *action = deviceMenu->addAction(QString::fromStdString(device.name));
			action->setCheckable(true);
			action->setChecked(device.is_active);
			action->setDisabled(device.is_active);
			action->setData(QString::fromStdString(device.id));
		}
	});
}

void MainMenu::deviceSelected(QAction *action)
{
	spotify.set_device(action->data().toString().toStdString(),
		[action](const std::string &status)
		{
			if (!status.empty())
			{
				action->setChecked(false);
				StatusMessage::error(QString("Failed to set device: %1")
					.arg(QString::fromStdString(status)));
			}
			else
			{
				action->setDisabled(true);
			}
		});
}

void MainMenu::logOut(bool /*checked*/)
{
	QMessageBox box(QMessageBox::Icon::Question,
		"Are you sure?",
		"Do you also want to clear your application credentials or only log out?");

	auto *clearAll = box.addButton("Clear everything",
		QMessageBox::ButtonRole::AcceptRole);
	auto *logOut = box.addButton("Only log out",
		QMessageBox::ButtonRole::AcceptRole);
	auto *cancel = box.addButton("Cancel",
		QMessageBox::ButtonRole::RejectRole);
	box.exec();
	auto *result = box.clickedButton();

	// Return if we pressed cancel
	if (result == nullptr || result == cancel)
	{
		return;
	}

	// Clear client secret/id if clearAll
	if (result == clearAll)
	{
		this->settings.remove_client();
	}

	// Clear login if cleatAll/logOut
	if (result == clearAll || result == logOut)
	{
		this->settings.remove_tokens();
	}
	this->settings.save();
	QMessageBox::information(MainWindow::find(parentWidget()),
		"Logged out",
		"You are now logged out, the application will now close");

	QCoreApplication::quit();
}

void MainMenu::checkForUpdate(const std::string &data)
{
	if (data.empty())
	{
		return;
	}

	auto json = nlohmann::json::parse(data);
	if (!json.contains("tag_name"))
	{
		return;
	}

	const auto &latest = json.at("tag_name").get<std::string>();
	auto isLatest = latest.empty() || latest == APP_VERSION;
	if (latest.empty() || isLatest)
	{
		about->setVisible(false);
	}
	else
	{
		about->setDisabled(false);
		about->setText(QString("Update found: %1")
			.arg(QString::fromStdString(latest)));
		QAction::connect(about, &QAction::triggered, [this]()
		{
			QString url("https://github.com/kraxarn/spotify-qt/releases/latest");
			Url::open(url, LinkType::Web, MainWindow::find(this->parentWidget()));
		});
	}
}

void MainMenu::onOpenSettings(bool /*checked*/)
{
	auto *parent = MainWindow::find(parentWidget());

	Dialog::Settings dialog(settings, cache, httpClient, parent);
	dialog.exec();
}
