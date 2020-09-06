#include "settingsdialog.hpp"

SettingsDialog::SettingsDialog(Settings &settings, QWidget *parent)
	: settings(settings), QDialog(parent)
{
	// Main layout
	auto mainLayout = new QVBoxLayout();
	auto tabs = new QTabWidget();

	// Tabs
	pages.append(
		{
			new AppSettings(settings, this),
			new InterfaceSettings(settings, this),
			new TraySettings(settings, this)
		}
	);

	for (auto page : pages)
	{
		tabs->addTab(page->toWidget(), page->title());
	}

	tabs->addTab(spotifySettings(), "Spotify");
	tabs->addTab(aboutSettings(), "About");
	mainLayout->addWidget(tabs, 1);

	// Buttons
	auto buttons = new QDialogButtonBox();
	auto okButton = buttons->addButton(QDialogButtonBox::Ok);
	QPushButton::connect(okButton, &QPushButton::clicked, [this](bool checked)
	{
		if (applySettings())
			accept();
	});
	auto applyButton = buttons->addButton(QDialogButtonBox::Apply);
	QPushButton::connect(applyButton, &QPushButton::clicked, [this](bool checked)
	{
		applySettings();
	});
	auto cancelButton = buttons->addButton(QDialogButtonBox::Cancel);
	QPushButton::connect(cancelButton, &QPushButton::clicked, [this](bool checked)
	{
		accept();
	});
	mainLayout->addWidget(buttons);

	// Set layout
	setWindowTitle("Settings");
	setLayout(mainLayout);
	resize(360, 260);
}

QWidget *SettingsDialog::spotifySettings()
{
	// Main container for everything
	auto layout = new QVBoxLayout();
	layout->setAlignment(Qt::AlignTop);

	// Executable settings
	auto sptPathLayout = new QHBoxLayout();
	sptPath = new QLineEdit(settings.spotify.path, this);
	sptPath->setPlaceholderText("spotifyd path");
	sptPathLayout->addWidget(sptPath, 1);
	auto sptPathBrowse = new QPushButton("...", this);
	sptPathBrowse->setMaximumWidth(40);
	sptPathBrowse->setFlat(true);
	QAbstractButton::connect(sptPathBrowse, &QAbstractButton::clicked, [this](bool checked)
	{
		auto filePath = QFileDialog::getOpenFileName(this, "spotifyd path", "/");
		if (!filePath.isEmpty() && sptPath != nullptr)
			sptPath->setText(filePath);
	});
	sptPathLayout->addWidget(sptPathBrowse);
	auto sptOpenLog = new QPushButton(this);
	sptOpenLog->setIcon(Icon::get("folder-txt"));
	sptOpenLog->setToolTip("Open log");
	sptOpenLog->setFlat(true);
	QAbstractButton::connect(sptOpenLog, &QAbstractButton::clicked, [this](bool checked)
	{
		(new ClientHandlerLogDialog(this->parentWidget()))->show();
	});
	sptPathLayout->addWidget(sptOpenLog);
	layout->addLayout(sptPathLayout);

	// Spotifyd version
	sptVersion = new QLabel("(no spotifyd provided)", this);
	if (!settings.spotify.path.isEmpty())
	{
		auto client = spt::ClientHandler::version(settings.spotify.path);
		if (sptVersion != nullptr)
			sptVersion->setText(client);
	}
	sptVersion->setEnabled(false);
	layout->addWidget(sptVersion);

	// Global config
	sptGlobal = new QCheckBox("Use global config", this);
	sptGlobal->setToolTip("Use spotifyd.conf file in either ~/.config/spotifyd or /etc/spotifyd only");
	sptGlobal->setChecked(settings.spotify.globalConfig);
	QCheckBox::connect(sptGlobal, &QCheckBox::stateChanged, this, &SettingsDialog::globalConfigToggle);
	layout->addWidget(sptGlobal);

	// Box and layout for all app specific settings
	sptGroup = new QGroupBox("App specific settings", this);
	sptGroup->setEnabled(!sptGlobal->isChecked());
	auto sptLayout = new QGridLayout();

	// Username
	sptLayout->addWidget(new QLabel("Username", sptGroup), 0, 0);
	sptUsername = new QLineEdit(settings.spotify.username, sptGroup);
	sptLayout->addWidget(sptUsername, 0, 1);

	// Bitrate
	sptLayout->addWidget(new QLabel("Quality", sptGroup), 1, 0);
	sptBitrate = new QComboBox(sptGroup);
	sptBitrate->addItems(
		{
			"Low (96 kbit/s)", "Medium (160 kbit/s)", "High (320 kbit/s)"
		}
	);
	auto bitrate = settings.spotify.bitrate;
	sptBitrate->setCurrentIndex(bitrate == 96 ? 0 : bitrate == 160 ? 1 : 2);
	sptLayout->addWidget(sptBitrate, 1, 1);
	sptGroup->setLayout(sptLayout);
	layout->addWidget(sptGroup);

	// Start with app
	sptAppStart = new QCheckBox("Start with app", this);
	sptAppStart->setToolTip("Start, and close, spotifyd together with the app (only closes with app config)");
	sptAppStart->setChecked(settings.spotify.startClient);
	layout->addWidget(sptAppStart);

	// Always start
	sptAlways = new QCheckBox("Always start", this);
	sptAlways->setToolTip("Always start client, even if there are other devices already available");
	sptAlways->setChecked(settings.spotify.alwaysStart);
	layout->addWidget(sptAlways);

	// Final layout
	auto widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}

void SettingsDialog::globalConfigToggle(int state)
{
	sptGroup->setEnabled(state == Qt::Unchecked);
}

QWidget *SettingsDialog::aboutSettings()
{
	auto cacheSize = 0u;
	auto mainWindow = dynamic_cast<MainWindow *>(parentWidget());
	if (mainWindow != nullptr)
		for (auto const &file : QDir(mainWindow->getCacheLocation()).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
			for (auto const &f : QDir(file.absoluteFilePath()).entryInfoList(QDir::Files))
				cacheSize += f.size();

	auto layout = new QVBoxLayout();
	layout->setAlignment(Qt::AlignTop);

	// Title
	auto title = new QHBoxLayout();
	title->setAlignment(Qt::AlignHCenter);
	auto titleLogo = new QLabel();
	titleLogo->setPixmap(Icon::get("logo:spotify-qt").pixmap(64, 64));
	title->addWidget(titleLogo);
	layout->addLayout(title);
	auto titleVersion = new QVBoxLayout();
	titleVersion->setSpacing(0);
	titleVersion->setAlignment(Qt::AlignVCenter);
	auto titleAppName = new QLabel("spotify-qt");
	auto appNameFont = titleAppName->font();
	appNameFont.setPointSize(appNameFont.pointSizeF() * 1.5);
	titleAppName->setFont(appNameFont);
	titleVersion->addWidget(titleAppName);
	titleVersion->addWidget(new QLabel(APP_VERSION));
	title->addLayout(titleVersion);

	// User info
	if (mainWindow != nullptr)
		layout->addWidget(new QLabel(QString("Hello %1!")
										 .arg(mainWindow->getCurrentUser().displayName)), 0, Qt::AlignHCenter);

	// Grid with buttons
	layout->addSpacing(8);
	auto options = new QGridLayout();

	// About Qt
	auto aboutQt = new QPushButton(Icon::get("logo:qt"),
								   QString("About Qt %1.%2")
									   .arg(QT_VERSION_MAJOR).arg(QT_VERSION_MINOR));
	aboutQt->setFlat(true);
	QAbstractButton::connect(aboutQt, &QPushButton::clicked, [this](bool checked)
	{
		QMessageBox::aboutQt(this);
	});
	options->addWidget(aboutQt);

	// Generate report
	auto generateReport = new QPushButton(Icon::get("description"), "System info", this);
	generateReport->setFlat(true);
	QAbstractButton::connect(generateReport, &QPushButton::clicked, [this, mainWindow](bool checked)
	{
		(new SystemInfoDialog(mainWindow, this))->show();
	});
	options->addWidget(generateReport, 0, 1);

	// Open cache directory
	auto openCache = new QPushButton(Icon::get("folder-temp"),
									 QString("Open cache directory (%1M)").arg(cacheSize / 1000 / 1000));
	openCache->setFlat(true);
	QAbstractButton::connect(openCache, &QPushButton::clicked, [this, mainWindow](bool checked)
	{
		if (mainWindow == nullptr)
			return;
		Utils::openUrl(mainWindow->getCacheLocation(), LinkType::Web, this);
	});
	options->addWidget(openCache);

	// Open config file
	auto openConfig = new QPushButton(Icon::get("folder-txt"), "Open config file");
	openConfig->setFlat(true);
	QAbstractButton::connect(openConfig, &QPushButton::clicked, [this](bool checked)
	{
		Utils::openUrl(Settings::fileName(), LinkType::Path, this);
	});
	options->addWidget(openConfig);
	layout->addLayout(options, 1);

	// Final layout
	return Utils::layoutToWidget(layout);
}

bool SettingsDialog::applySettings()
{
	auto window = dynamic_cast<MainWindow *>(parent());

	for (auto &page : pages)
	{
		if (!page->applySettings(window))
			return false;
	}

	// TODO: Save settings
	//return true; // TODO

	// Check spotify client path
	if (!sptPath->text().isEmpty())
	{
		auto client = spt::ClientHandler::version(sptPath->text());
		if (client.isEmpty())
		{
			applyFail("spotifyd path");
			return false;
		}
		sptVersion->setText(client);
		settings.spotify.path = sptPath->text();
	}

	// Spotify global config
	if (sptGlobal->isChecked() && !sptConfigExists())
		QMessageBox::warning(this,
							 "spotifyd config not found",
							 QString("Couldn't find a config file for spotifyd. You may experience issues."));
	settings.spotify.globalConfig = sptGlobal->isChecked();

	// Other Spotify stuff
	settings.spotify.startClient = sptAppStart->isChecked();
	settings.spotify.username = sptUsername->text();
	auto bitrate = sptBitrate->currentIndex();
	settings.spotify.bitrate = bitrate == 0 ? 96 : bitrate == 1 ? 160 : 320;
	settings.spotify.alwaysStart = sptAlways->isChecked();

	// Everything is fine
	settings.save();
	return true;
}

bool SettingsDialog::sptConfigExists()
{
	// Config is either ~/.config/spotifyd/spotifyd.conf or /etc/spotifyd/spotifyd.conf
	return QFile(QString("%1/.config/spotifyd/spotifyd.conf")
					 .arg(QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0])).exists()
		|| QFile("/etc/spotifyd/spotifyd.conf").exists();
}