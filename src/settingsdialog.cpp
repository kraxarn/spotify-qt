#include "settingsdialog.hpp"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
	// Main layout
	auto mainLayout = new QVBoxLayout();
	auto tabs = new QTabWidget();
	tabs->addTab(appSettings(), "Application");
	tabs->addTab(interfaceSettings(), "Interface");
	tabs->addTab(spotifySettings(), "Spotify");
	tabs->addTab(aboutSettings(), "About");
	mainLayout->addWidget(tabs, 1);
	// Buttons
	auto buttons = new QDialogButtonBox();
	auto okButton = buttons->addButton(QDialogButtonBox::Ok);
	QPushButton::connect(okButton, &QPushButton::clicked, [=](bool checked) {
		if (applySettings())
			accept();
	});
	auto applyButton = buttons->addButton(QDialogButtonBox::Apply);
	QPushButton::connect(applyButton, &QPushButton::clicked, [=](bool checked) {
		applySettings();
	});
	auto cancelButton = buttons->addButton(QDialogButtonBox::Cancel);
	QPushButton::connect(cancelButton, &QPushButton::clicked, [=](bool checked) {
		accept();
	});
	mainLayout->addWidget(buttons);
	// Set layout
	setWindowTitle("Settings");
	setLayout(mainLayout);
	resize(360, 260);
}

QWidget *SettingsDialog::appSettings()
{
	auto layout = new QVBoxLayout();
	layout->setAlignment(Qt::AlignTop);
	// Refresh interval
	auto appRefreshLayout = new QHBoxLayout();
	auto appRefreshLabel = new QLabel("Refresh interval", this);
	appRefreshLabel->setToolTip("How often to refresh playback status from the Spotify servers");
	appRefreshLayout->addWidget(appRefreshLabel);
	appRefresh = new QComboBox(this);
	appRefresh->addItems({
		"1", "3", "10"
	});
	appRefresh->setEditable(true);
	appRefresh->setCurrentIndex(-1);
	appRefresh->setEditText(QString::number(settings.refreshInterval()));
	appRefresh->setValidator(new QIntValidator(1, 60, this));
	appRefreshLayout->addWidget(appRefresh);
	appRefreshLayout->addWidget(new QLabel("seconds"));
	layout->addLayout(appRefreshLayout);
	// PulseAudio volume control
	if (isPulse())
	{
		appPulse = new QCheckBox("PulseAudio volume control", this);
		appPulse->setToolTip(
			"Use PulseAudio for volume control instead, only works if listening on same device");
		appPulse->setChecked(settings.pulseVolume());
		layout->addWidget(appPulse);
	}
	// MPRIS D-Bus
	appMedia = new QCheckBox("Media controller", this);
	appMedia->setChecked(settings.mediaController());
#ifdef Q_OS_LINUX
	appMedia->setToolTip("Enable media controller through the MPRIS D-Bus interface");
#else
	appMedia->setToolTip("Currently only available on Linux");
	appMedia->setEnabled(false);
#endif
	layout->addWidget(appMedia);
	// Spotify playback order
	appSptOrder = new QCheckBox("Spotify playback order", this);
	appSptOrder->setToolTip("Use Spotify playback order instead of app list order");
	appSptOrder->setChecked(settings.sptPlaybackOrder());
	layout->addWidget(appSptOrder);
	// Final layout
	auto widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}

QWidget *SettingsDialog::interfaceSettings()
{
	auto layout = new QVBoxLayout();
	layout->setAlignment(Qt::AlignTop);
	// Dark theme
	itfDark = new QCheckBox("Dark theme", this);
	itfDark->setToolTip("Use custom dark theme");
	itfDark->setChecked(settings.darkTheme());
	layout->addWidget(itfDark);
	// Tray icon settings
	itfTrayIcon = new QCheckBox("Tray icon", this);
	itfTrayIcon->setToolTip("Add an icon to the system tray for quick access");
	itfTrayIcon->setChecked(settings.trayIcon());
	layout->addWidget(itfTrayIcon);
	// Desktop notifications
	itfTrayNotify = new QCheckBox("Desktop notifications", this);
	itfTrayNotify->setToolTip("Replace status bar with desktop notifications (suppresses any non-error messages)");
	itfTrayNotify->setChecked(settings.trayNotifications());
	layout->addWidget(itfTrayNotify);
	QCheckBox::connect(itfTrayNotify, &QCheckBox::stateChanged, [this](int state) {
		if (state == Qt::CheckState::Checked && itfTrayIcon != nullptr) {
			itfTrayIcon->setChecked(true);
		}
	});
	// Invert tray icon
	itfTrayInvert = new QCheckBox("Invert tray icon", this);
	itfTrayInvert->setToolTip("Invert colors in tray icon to be visible on light backgrounds");
	itfTrayInvert->setChecked(settings.trayLightIcon());
	layout->addWidget(itfTrayInvert);
	// Song header resize mode
	itfResizeAuto = new QCheckBox("Auto resize track list headers", this);
	itfResizeAuto->setToolTip("Automatically resize track list headers to fit content");
	itfResizeAuto->setChecked(settings.songHeaderResizeMode() == QHeaderView::ResizeToContents);
	layout->addWidget(itfResizeAuto);
	// Final layout
	auto widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}

QWidget *SettingsDialog::spotifySettings()
{
	// Main container for everything
	auto layout = new QVBoxLayout();
	layout->setAlignment(Qt::AlignTop);
	// Executable settings
	auto sptPathLayout = new QHBoxLayout();
	sptPath = new QLineEdit(settings.sptPath(), this);
	sptPath->setPlaceholderText("spotifyd path");
	sptPathLayout->addWidget(sptPath, 1);
	auto sptPathBrowse = new QPushButton("...", this);
	sptPathBrowse->setMaximumWidth(40);
	sptPathBrowse->setFlat(true);
	QAbstractButton::connect(sptPathBrowse, &QAbstractButton::clicked, [this](bool checked) {
		auto filePath = QFileDialog::getOpenFileName(this, "spotifyd path", "/");
		if (!filePath.isEmpty() && sptPath != nullptr)
			sptPath->setText(filePath);
	});
	sptPathLayout->addWidget(sptPathBrowse);
	layout->addLayout(sptPathLayout);
	// Spotifyd version
	sptVersion = new QLabel("(no spotifyd provided)", this);
	if (!settings.sptPath().isEmpty())
	{
		auto client = spt::ClientHandler::version(settings.sptPath());
		if (sptVersion != nullptr)
			sptVersion->setText(client);
	}
	sptVersion->setEnabled(false);
	layout->addWidget(sptVersion);
	// Layout for all settings
	auto sptLayout = new QGridLayout();
	sptLayout->setEnabled(false);
	layout->addLayout(sptLayout);
	// Username
	sptLayout->addWidget(new QLabel("Username", this), 0, 0);
	sptUsername = new QLineEdit(settings.sptUser(), this);
	sptLayout->addWidget(sptUsername, 0, 1);
	// Bitrate
	sptLayout->addWidget(new QLabel("Quality", this));
	sptBitrate = new QComboBox(this);
	sptBitrate->addItems({
		"Low (96 kbit/s)", "Medium (160 kbit/s)", "High (320 kbit/s)"
	});
	auto bitrate = settings.sptBitrate();
	sptBitrate->setCurrentIndex(bitrate == 96 ? 0 : bitrate == 160 ? 1 : 2);
	sptLayout->addWidget(sptBitrate);
	// Start with app
	sptAppStart = new QCheckBox("Start with app", this);
	sptAppStart->setToolTip("Start spotifyd together with the app (always starts and doesn't automatically close)");
	sptAppStart->setChecked(settings.sptStartClient());
	layout->addWidget(sptAppStart);
	// Global config
	sptGlobal = new QCheckBox("Use global config", this);
	sptGlobal->setToolTip("Use spotifyd.conf file in either ~/.config/spotifyd or /etc/spotifyd only");
	sptGlobal->setChecked(settings.sptGlobalConfig());
	layout->addWidget(sptGlobal);
	// Final layout
	auto widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}

QWidget *SettingsDialog::aboutSettings()
{
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
	// Grid with buttons
	layout->addSpacing(8);
	auto options = new QGridLayout();
	// About Qt
	auto aboutQt = new QPushButton(Icon::get("logo:qt"),
		QString("About Qt %1.%2")
			.arg(QT_VERSION_MAJOR).arg(QT_VERSION_MINOR));
	aboutQt->setFlat(true);
	QAbstractButton::connect(aboutQt, &QPushButton::clicked, [this](bool checked) {
		QMessageBox::aboutQt(this);
	});
	options->addWidget(aboutQt);
	// Check for updates
	auto checkUpdates = new QPushButton(Icon::get("download"), "Check for updates");
	checkUpdates->setFlat(true);
	QAbstractButton::connect(checkUpdates, &QPushButton::clicked, [this, checkUpdates](bool checked) {
		auto window = dynamic_cast<MainWindow*>(parent());
		if (window == nullptr)
			return;
		checkUpdates->setEnabled(false);
		checkUpdates->setText("Please wait...");
		auto json = window->getJson("https://api.github.com/repos/kraxarn/spotify-qt/releases");
		auto latest = json.array()[0].toObject()["tag_name"].toString();
		auto isLatest = latest == APP_VERSION;
		checkUpdates->setText(isLatest
			? "None found"
			: "Update found");
		if (!isLatest)
			window->setStatus(
				QString("Update found, latest version is %1, you have version %2")
					.arg(latest)
					.arg(APP_VERSION), true);
		checkUpdates->setEnabled(true);
	});
	options->addWidget(checkUpdates, 0, 1);
	// Open cache directory
	auto openCache = new QPushButton(Icon::get("folder-temp"), "Open cache directory");
	openCache->setFlat(true);
	QAbstractButton::connect(openCache, &QPushButton::clicked, [this](bool checked) {
		auto window = dynamic_cast<MainWindow*>(parent());
		if (window == nullptr)
			return;
		if (!QDesktopServices::openUrl(QUrl(window->cacheLocation)))
			QMessageBox::warning(this,
				"No path",
				QString("Failed to open path: %1").arg(window->cacheLocation));
	});
	options->addWidget(openCache);
	// Open config file
	auto openConfig = new QPushButton(Icon::get("folder-txt"), "Open config file");
	openConfig->setFlat(true);
	QAbstractButton::connect(openConfig, &QPushButton::clicked, [this](bool checked) {
		if (!QDesktopServices::openUrl(QUrl(Settings().fileName())))
			QMessageBox::warning(this,
				"No file",
				QString("Failed to open file: %1").arg(Settings().fileName()));
	});
	options->addWidget(openConfig);
	layout->addLayout(options, 1);
	// Final layout
	auto widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}

bool SettingsDialog::applySettings()
{
	// Set theme
	auto changeTheme = itfDark->isChecked() != settings.darkTheme();
	if (changeTheme)
	{
		QMessageBox::information(
			this, "Dark Theme",
			"Please restart the application to fully apply selected theme");
		settings.setDarkTheme(itfDark->isChecked());
		QApplication::setStyle(settings.style());
		MainWindow::applyPalette(settings.stylePalette());
	}

	// Media controller
	if (appMedia->isChecked() != settings.mediaController())
		QMessageBox::information(
			this, "Media Controller",
			"Please restart the application to apply changes");
	settings.setMediaController(appMedia->isChecked());

	// PulseAudio volume
	if (appPulse != nullptr)
		settings.setPulseVolume(appPulse->isChecked());

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
		settings.setSptPath(sptPath->text());
	}

	// Desktop notifications and tray icon
	if (itfTrayNotify->isChecked() && !itfTrayIcon->isChecked())
	{
		itfTrayIcon->setChecked(true);
		QMessageBox::information(
			this, "Desktop Notifications",
			"Desktop notifications requires tray icon to be enabled, so it was enabled");
	}
	// Check if tray icon needs to be reloaded
	auto reloadTray = settings.trayIcon() != itfTrayIcon->isChecked()
		|| settings.trayNotifications() != itfTrayNotify->isChecked()
		|| settings.trayLightIcon() != itfTrayInvert->isChecked();
	// Apply
	settings.setTrayIcon(itfTrayIcon->isChecked());
	settings.setTrayNotifications(itfTrayNotify->isChecked());
	settings.setTrayLightIcon(itfTrayInvert->isChecked());
	// Reload if needed
	auto window = dynamic_cast<MainWindow*>(parent());
	if (reloadTray && window != nullptr)
		window->reloadTrayIcon();
	// Song header resize mode
	auto resizeMode = itfResizeAuto->isChecked()
		? QHeaderView::ResizeToContents
		: QHeaderView::Interactive;
	if (resizeMode != settings.songHeaderResizeMode() && window != nullptr)
		window->songs->header()->setSectionResizeMode(resizeMode);
	settings.setSongHeaderResizeMode(resizeMode);

	// Refresh interval
	auto ok = false;
	auto interval = appRefresh->currentText().toInt(&ok);
	if (!ok || interval <= 0 || interval > 60)
	{
		applyFail("refresh interval");
		return false;
	}
	settings.setRefreshInterval(interval);

	// Spotify global config
	if (sptGlobal->isChecked() && !sptConfigExists())
		QMessageBox::warning(this,
			"spotifyd config not found",
			QString("Couldn't find a config file for spotifyd. You may experience issues."));
	settings.setSptGlobalConfig(sptGlobal->isChecked());

	// Other Spotify stuff
	settings.setSptPlaybackOrder(appSptOrder->isChecked());
	settings.setSptStartClient(sptAppStart->isChecked());
	settings.setSptUser(sptUsername->text());
	auto bitrate = sptBitrate->currentIndex();
	settings.setSptBitrate(bitrate == 0 ? 96 : bitrate == 1 ? 160 : 320);

	// Everything is fine
	return true;
}

void SettingsDialog::applyFail(const QString &setting)
{
	QMessageBox::warning(this,
		"Failed to apply settings",
		QString("Failed to apply setting \"%1\". Check your settings and try again.").arg(setting));
}

bool SettingsDialog::isPulse()
{
	// Assume /usr/bin/pactl
	return QFileInfo("/usr/bin/pactl").isExecutable();
}

bool SettingsDialog::sptConfigExists()
{
	// Config is either ~/.config/spotifyd/spotifyd.conf or /etc/spotifyd/spotifyd.conf
	return QFile(QString("%1/.config/spotifyd/spotifyd.conf")
		.arg(QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0])).exists()
		|| QFile("/etc/spotifyd/spotifyd.conf").exists();
}
