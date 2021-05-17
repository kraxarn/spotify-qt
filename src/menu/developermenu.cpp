#include "developermenu.hpp"

DeveloperMenu::DeveloperMenu(lib::settings &settings, lib::spt::api &spotify,
	lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	settings(settings),
	cache(cache),
	QMenu("Developer", parent)
{
	setIcon(Icon::get("folder-txt"));

	QAction::connect(addAction("Test API requests"), &QAction::triggered, [this]()
	{
		auto *mainWindow = MainWindow::find(parentWidget());
		mainWindow->addSidePanelTab(new DebugView(this->settings, mainWindow), "API request");
	});

	QAction::connect(addAction("Reset size"), &QAction::triggered, [this]()
	{
		MainWindow::find(parentWidget())->resize(MainWindow::defaultSize());
	});

	QAction::connect(addAction("Refresh access token"), &QAction::triggered, [this]()
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

	addMenu(infoMenu());
	addMenu(dialogMenu());
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
		new WhatsNewDialog(APP_VERSION, settings, mainWindow),
	};

	for (auto *dialog : dialogs)
	{
		QAction::connect(menu->addAction(dialog->metaObject()->className()),
			&QAction::triggered, [dialog]()
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

	QAction::connect(menu->addAction("Playback"), &QAction::triggered,
		[mainWindow]()
		{
			nlohmann::json json = mainWindow->currentPlayback();
			QMessageBox::information(mainWindow, "Playback",
				QString::fromStdString(json.dump(4)));
		});

	QAction::connect(menu->addAction("Context"), &QAction::triggered,
		[mainWindow]()
		{
			QMessageBox::information(mainWindow, "Context",
				QString::fromStdString(mainWindow->getSptContext()));
		});

	return menu;
}
