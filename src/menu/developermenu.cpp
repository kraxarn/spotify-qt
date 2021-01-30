#include "developermenu.hpp"

DeveloperMenu::DeveloperMenu(lib::settings &settings, QWidget *parent)
	: settings(settings),
	QMenu("Developer", parent)
{
	setIcon(Icon::get("folder-txt"));

	QAction::connect(addAction("Test API requests"), &QAction::triggered, [this]()
	{
		auto mainWindow = MainWindow::find(parentWidget());
		mainWindow->addSidePanelTab(new DebugView(this->settings, mainWindow), "API request");
	});

	addMenu(infoMenu());
	addMenu(dialogMenu());
}

QMenu *DeveloperMenu::dialogMenu()
{
	auto menu = new QMenu("Dialogs", this);
	auto mainWindow = MainWindow::find(parentWidget());

	std::vector<QDialog *> dialogs = {
		new DeviceSelectDialog(QVector<spt::Device>(), mainWindow),
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
			QMessageBox::information(mainWindow, "Context", mainWindow->getSptContext());
		});

	return menu;
}
