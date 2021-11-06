#include "developermenu.hpp"
#include "mainwindow.hpp"

DeveloperMenu::DeveloperMenu(lib::settings &settings, lib::spt::api &spotify,
	lib::cache &cache, const lib::http_client &httpClient, QWidget *parent)
	: QMenu("Developer", parent),
	settings(settings),
	spotify(spotify),
	cache(cache),
	httpClient(httpClient)
{
	setIcon(Icon::get("folder-txt"));

	addMenuItem(this, "Test API requests", [this]()
	{
		auto *mainWindow = MainWindow::find(parentWidget());
		auto *debugView = new DebugView(this->settings, mainWindow);
		mainWindow->addSidePanelTab(debugView, "API request");
	});

	addMenuItem(this, "Reset size", [this]()
	{
		MainWindow::find(parentWidget())->resize(MainWindow::defaultSize());
	});

	addMenuItem(this, "Refresh access token", [this]()
	{
		try
		{
			this->spotify.refresh();
			QMessageBox::information(this, "Success",
				QString::fromStdString(lib::fmt::format("Successfully refreshed access token:\n{}",
					this->settings.account.refresh_token)));
		}
		catch (const std::exception &e)
		{
			QMessageBox::critical(this, "Error",
				QString("Refresh failed: %1").arg(e.what()));
		}
	});

	addMenuItem(this, "Load huge playlist (very slow)", [this]()
	{
		// playlist id taken from https://github.com/librespot-org/librespot/issues/481
		this->spotify.playlist("0bQjBPRqy5zReJethJy3aP",
			[this](const lib::spt::playlist &playlist)
			{
				auto *mainWindow = MainWindow::find(this->parentWidget());
				mainWindow->getSongsTree()->load(playlist);
			});
	});

	addMenuItem(this, "Show track notification", [this]()
	{
		auto *mainWindow = MainWindow::find(parentWidget());

		auto *trayIcon = mainWindow->getTrayIcon();
		if (trayIcon == nullptr)
		{
			StatusMessage::warn(QStringLiteral("Tray icon not loaded"));
			return;
		}

		const auto &track = mainWindow->currentPlayback().item;

		HttpUtils::getAlbum(track.image, this->httpClient, this->cache,
			[trayIcon, &track](const QPixmap &pixmap)
			{
				trayIcon->message(track, pixmap);
			});
	});

	addMenu(infoMenu());
	addMenu(dialogMenu());
	addMenu(crashMenu());
	addMenu(statusMenu());

#ifdef USE_DBUS
	addMenu(notificationsMenu());
#endif
}

void DeveloperMenu::addMenuItem(QMenu *menu, const QString &text,
	const std::function<void()> &triggered)
{
	auto *action = menu->addAction(text);
	QAction::connect(action, &QAction::triggered, [triggered](bool /*checked*/)
	{
		triggered();
	});
}

auto DeveloperMenu::dialogMenu() -> QMenu *
{
	auto *menu = new QMenu("Dialogs", this);
	auto *mainWindow = MainWindow::find(parentWidget());

	std::vector<QDialog *> dialogs = {
		new DeviceSelectDialog({}, mainWindow),
		new OpenLinkDialog("/", LinkType::Path, mainWindow),
		new SetupDialog(settings, mainWindow),
		new TracksCacheDialog(cache, mainWindow),
		new WhatsNewDialog(settings, httpClient, mainWindow),
	};

	for (auto *dialog: dialogs)
	{
		addMenuItem(menu, dialog->metaObject()->className(), [dialog]()
		{
			dialog->open();
		});
	}

	return menu;
}

auto DeveloperMenu::infoMenu() -> QMenu *
{
	auto *menu = new QMenu("Current", this);
	auto *mainWindow = MainWindow::find(parentWidget());

	addMenuItem(menu, "Playback", [mainWindow]()
	{
		nlohmann::json json = mainWindow->currentPlayback();
		QMessageBox::information(mainWindow, "Playback",
			QString::fromStdString(json.dump(4)));
	});

	addMenuItem(menu, "Playback (MPRIS)", [mainWindow]()
	{
		nlohmann::json json = mainWindow->currentPlayback().metadata();
		QMessageBox::information(mainWindow, "Metadata",
			QString::fromStdString(json.dump(4)));
	});

	addMenuItem(menu, "Context", [mainWindow]()
	{
		QMessageBox::information(mainWindow, "Context",
			QString::fromStdString(mainWindow->getSptContext()));
	});

	return menu;
}

auto DeveloperMenu::crashMenu() -> QMenu *
{
	auto *menu = new QMenu("Crash", this);

	addMenuItem(menu, "exception", []()
	{
		throw std::runtime_error("debug crash");
	});

	addMenuItem(menu, "segfault", []()
	{
		// Do stuff with widget that hopefully doesn't exist
		// to avoid any "unexpected behaviour" warnings
		QWidget::find(-1)->update();
	});

	addMenuItem(menu, "qFatal", []()
	{
		qFatal("debug crash");
	});

	return menu;
}

auto DeveloperMenu::statusMenu() -> QMenu *
{
	auto *menu = new QMenu("Status message", this);

	addMenuItem(menu, "Information", []()
	{
		StatusMessage::show(MessageType::Information, "Information");
	});

	addMenuItem(menu, "Warning", []()
	{
		StatusMessage::show(MessageType::Warning, "Warning");
	});

	addMenuItem(menu, "Error", []()
	{
		StatusMessage::show(MessageType::Error, "Error");
	});

	return menu;
}

#ifdef USE_DBUS

auto DeveloperMenu::notificationsMenu() -> QMenu *
{
	auto *menu = new QMenu("Notifications", this);
	auto *mainWindow = MainWindow::find(parentWidget());

	addMenuItem(menu, "Capabilities", [mainWindow]()
	{
		DbusNotifications notifications;
		QMessageBox::information(mainWindow, QStringLiteral("Capabilities"),
			notifications.getCapabilities().join('\n'));
	});

	addMenuItem(menu, "Notify", []()
	{
		DbusNotifications notifications;
		notifications.notify(QStringLiteral("Title"),
			QStringLiteral("<b>Bold text</b><br/>Normal text"),
			QString(),
			-1);
	});

	return menu;
}

#endif
