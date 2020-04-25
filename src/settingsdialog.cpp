#include "settingsdialog.hpp"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
	// Main layout
	auto mainLayout = new QVBoxLayout();
	auto tabs = new QTabWidget();
	tabs->addTab(appSettings(), "Application");
	tabs->addTab(interfaceSettings(), "Interface");
	tabs->addTab(spotifySettings(), "Spotify");
	mainLayout->addWidget(tabs);
	// Buttons
	auto okButton = new QPushButton("OK");
	QPushButton::connect(okButton, &QPushButton::clicked, [=](bool checked) {
		if (applySettings())
			accept();
	});
	auto applyButton = new QPushButton("Apply");
	QPushButton::connect(applyButton, &QPushButton::clicked, [=](bool checked) {
		applySettings();
	});
	auto cancelButton = new QPushButton("Cancel");
	QPushButton::connect(cancelButton, &QPushButton::clicked, [=](bool checked) {
		accept();
	});
	auto buttons = new QHBoxLayout();
	buttons->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	buttons->addWidget(okButton);
	buttons->addWidget(applyButton);
	buttons->addWidget(cancelButton);
	mainLayout->addLayout(buttons);
	// Set layout
	setWindowTitle("Settings");
	setLayout(mainLayout);
	mainLayout->setSizeConstraint(QLayout::SetFixedSize);
}

QWidget *SettingsDialog::appSettings()
{
	auto appLayout = new QVBoxLayout();
	// Refresh interval
	auto appRefreshLabel = new QLabel("Refresh interval", this);
	appRefreshLabel->setToolTip("How often to refresh playback status from the Spotify servers");
	appLayout->addWidget(appRefreshLabel);
	auto appRefresh = new QComboBox(this);
	appRefresh->addItems({
		"Fast (3s)", "Medium (5s)", "Slow (10s)"
	});
	appRefresh->setCurrentIndex(1);
	appLayout->addWidget(appRefresh);
	appRefresh->hide();
	appRefreshLabel->hide();
	// Start client
	sptStartClient = new QCheckBox("Autostart spotifyd", this);
	sptStartClient->setToolTip("Start spotifyd together with the app");
	sptStartClient->setChecked(settings.sptStartClient());
	appLayout->addWidget(sptStartClient);
	// PulseAudio volume control
	if (isPulse())
	{
		appPulse = new QCheckBox("PulseAudio volume control", this);
		appPulse->setToolTip(
			"Use PulseAudio for volume control instead, only works if listening on same device");
		appPulse->setChecked(settings.pulseVolume());
		appLayout->addWidget(appPulse);
	}
	// MPRIS D-Bus
	appMedia = new QCheckBox("Media controller", this);
	appMedia->setChecked(settings.mediaController());
#ifdef Q_OS_LINUX
	appMedia->setToolTip("Enable media controller through the MPRIS D-Bus interface, buggy (issue #4)");
#else
	appMedia->setToolTip("Currently only available on Linux");
	appMedia->setEnabled(false);
#endif
	appLayout->addWidget(appMedia);
	// Spotify playback order
	sptOrder = new QCheckBox("Spotify playback order", this);
	sptOrder->setToolTip("Use Spotify playback order instead of app list order");
	sptOrder->setChecked(settings.sptPlaybackOrder());
	appLayout->addWidget(sptOrder);
	// Filler
	appLayout->addStretch(1);
	// Final layout
	auto widget = new QWidget();
	widget->setLayout(appLayout);
	return widget;
}

QWidget *SettingsDialog::interfaceSettings()
{
	auto layout = new QVBoxLayout();
	// Dark theme
	darkTheme = new QCheckBox("Dark theme", this);
	darkTheme->setToolTip("Use custom dark theme");
	darkTheme->setChecked(settings.darkTheme());
	layout->addWidget(darkTheme);
	// Tray icon settings
	appTrayIcon = new QCheckBox("Tray icon", this);
	appTrayIcon->setToolTip("Add an icon to the system tray for quick access");
	appTrayIcon->setChecked(settings.trayIcon());
	layout->addWidget(appTrayIcon);
	// Desktop notifications
	appTrayNotify = new QCheckBox("Desktop notifications", this);
	appTrayNotify->setToolTip("Replace status bar with desktop notifications (suppresses any non-error messages)");
	appTrayNotify->setChecked(settings.trayNotifications());
	layout->addWidget(appTrayNotify);
	QCheckBox::connect(appTrayNotify, &QCheckBox::stateChanged, [this](int state) {
		if (state == Qt::CheckState::Checked && appTrayIcon != nullptr) {
			appTrayIcon->setChecked(true);
		}
	});
	// Filler
	layout->addStretch(1);
	// Final layout
	auto widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}

QWidget *SettingsDialog::spotifySettings()
{
	// Main container for everything
	auto sptMainLayout = new QVBoxLayout();
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
	sptMainLayout->addLayout(sptPathLayout);
	// Spotifyd version
	sptVersion = new QLabel("(no spotifyd provided)", this);
	if (!settings.sptPath().isEmpty())
	{
		auto client = sptClient(settings.sptPath());
		if (sptVersion != nullptr)
			sptVersion->setText(client);
	}
	sptVersion->setEnabled(false);
	sptMainLayout->addWidget(sptVersion);
	// Layout for all settings
	auto sptLayout = new QGridLayout();
	sptLayout->setEnabled(false);
	sptMainLayout->addLayout(sptLayout);
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
	// Filler
	sptMainLayout->addStretch(1);
	// Final layout
	auto widget = new QWidget();
	widget->setLayout(sptMainLayout);
	return widget;
}

QString SettingsDialog::sptClient(const QString &path)
{
	// Check if it exists
	QFileInfo file(path);
	if (!file.exists())
		return QString();
	// Check if either client
	if (file.baseName() != "spotifyd")
		return QString();
	// Prepare process
	QProcess process;
	// Get version info
	process.start(file.absoluteFilePath(), {
		"--version"
	});
	process.waitForFinished();
	// Entire stdout is version
	return process.readAllStandardOutput().trimmed();
}

bool SettingsDialog::applySettings()
{
	// Set theme
	auto changeTheme = darkTheme->isChecked() != settings.darkTheme();
	if (changeTheme)
	{
		QMessageBox::information(
			this, "Dark Theme",
			"Please restart the application to fully apply selected theme");
		settings.setDarkTheme(darkTheme->isChecked());
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
		auto client = sptClient(sptPath->text());
		if (client.isEmpty())
		{
			applyFail("spotifyd path");
			return false;
		}
		sptVersion->setText(client);
		settings.setSptPath(sptPath->text());
	}

	// Desktop notifications and tray icon
	if (appTrayNotify->isChecked() && !appTrayIcon->isChecked())
	{
		appTrayIcon->setChecked(true);
		QMessageBox::information(
			this, "Desktop Notifications",
			"Desktop notifications requires tray icon to be enabled, so it was enabled");
	}
	settings.setTrayIcon(appTrayIcon->isChecked());
	settings.setTrayNotifications(appTrayNotify->isChecked());

	// Other Spotify stuff
	settings.setSptPlaybackOrder(sptOrder->isChecked());
	settings.setSptStartClient(sptStartClient->isChecked());
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
