#include "developermenu.hpp"

DeveloperMenu::DeveloperMenu(lib::settings &settings, lib::spt::spotify_api &spotify,
	QWidget *parent)
	: spotify(spotify),
	settings(settings),
	QMenu("Developer", parent)
{
	setIcon(Icon::get("folder-txt"));

	QAction::connect(addAction("Test API requests"), &QAction::triggered, [this]()
	{
		auto mainWindow = MainWindow::find(parentWidget());
		mainWindow->addSidePanelTab(new DebugView(this->settings, mainWindow), "API request");
	});

	QAction::connect(addAction("Reset size"), &QAction::triggered, [this]()
	{
		MainWindow::find(parentWidget())->resize(1280, 720);
	});

	QAction::connect(addAction("Refresh access token"), &QAction::triggered, [this]()
	{
		if (this->spotify.refresh())
		{
			QMessageBox::information(this, "Success",
				QString::fromStdString(lib::fmt::format(
					"Successfully refreshed access token:\n{}",
					this->settings.account.refresh_token)));
		}
		else
		{
			QMessageBox::critical(this, "Error",
				"Refresh failed, check the log for details");
		}
	});

	addMenu(infoMenu());
	addMenu(dialogMenu());
}

QMenu *DeveloperMenu::dialogMenu()
{
	auto menu = new QMenu("Dialogs", this);
	auto mainWindow = MainWindow::find(parentWidget());

	std::vector<QDialog *> dialogs = {
		new DeviceSelectDialog({}, mainWindow),
		new OpenLinkDialog("/", LinkType::Path, mainWindow),
		new SetupDialog(settings, mainWindow),
		new TracksCacheDialog(mainWindow),
		new WhatsNewDialog(APP_VERSION, settings, mainWindow),
	};

	for (auto dialog : dialogs)
	{
		QAction::connect(menu->addAction(dialog->metaObject()->className()),
			&QAction::triggered, [dialog]()
			{
				dialog->open();
			});
	}

	return menu;
}

QMenu *DeveloperMenu::infoMenu()
{
	auto menu = new QMenu("Current", this);
	auto mainWindow = MainWindow::find(parentWidget());

	QAction::connect(menu->addAction("Playback"), &QAction::triggered,
		[mainWindow]()
		{
			QMessageBox::information(mainWindow, "Playback",
				QJsonDocument(mainWindow->currentPlayback().toJson())
					.toJson(QJsonDocument::Indented));
		});

	QAction::connect(menu->addAction("Context"), &QAction::triggered,
		[mainWindow]()
		{
			QMessageBox::information(mainWindow, "Context",
				QString::fromStdString(mainWindow->getSptContext()));
		});

	return menu;
}
