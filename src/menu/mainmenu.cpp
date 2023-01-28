#include "mainmenu.hpp"
#include "mainwindow.hpp"
#include "util/menuaction.hpp"
#include "util/shortcut.hpp"
#include "menu/developermenu.hpp"
#include "menu/device.hpp"

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
	if (settings.general.check_for_updates)
	{
		httpClient.get("https://api.github.com/repos/kraxarn/spotify-qt/releases/latest",
			lib::headers(), [this](const std::string &data)
			{
				this->checkForUpdate(data);
			});
	}
	else
	{
		about->setVisible(false);
	}

	// Device selection
	auto *deviceMenu = new Menu::Device(spotify, this);
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

	auto *logOutAction = MenuAction::create(QStringLiteral("im-user-away"),
		QStringLiteral("Log out..."), this, Shortcut::logOut());
	QAction::connect(logOutAction, &QAction::triggered,
		this, &MainMenu::logOut);

	addActions({
		logOutAction, quitAction
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
	if (settingsDialog == nullptr)
	{
		auto *parent = MainWindow::find(parentWidget());
		settingsDialog = new Dialog::Settings(settings, cache, httpClient, parent);
	}
	settingsDialog->open();
}
