#include "settingsdialog.hpp"

SettingsDialog::SettingsDialog(Settings &settings, QWidget *parent) : settings(settings), QDialog(parent)
{
	// Main layout
	auto mainLayout = new QVBoxLayout();
	auto tabs = new QTabWidget();
	tabs->addTab(appSettings(), "Application");
	tabs->addTab(interfaceSettings(), "Interface");
	tabs->addTab(traySettings(), "Tray icon");
	tabs->addTab(spotifySettings(), "Spotify");
	tabs->addTab(playlistSettings(), "Playlists");
	tabs->addTab(aboutSettings(), "About");
	mainLayout->addWidget(tabs, 1);

	// Buttons
	auto buttons = new QDialogButtonBox();
	auto okButton = buttons->addButton(QDialogButtonBox::Ok);
	QPushButton::connect(okButton, &QPushButton::clicked, [this](bool checked) {
		if (applySettings())
			accept();
	});
	auto applyButton = buttons->addButton(QDialogButtonBox::Apply);
	QPushButton::connect(applyButton, &QPushButton::clicked, [this](bool checked) {
		applySettings();
	});
	auto cancelButton = buttons->addButton(QDialogButtonBox::Cancel);
	QPushButton::connect(cancelButton, &QPushButton::clicked, [this](bool checked) {
		accept();
	});
	mainLayout->addWidget(buttons);

	// Set layout
	setWindowTitle("Settings");
	setLayout(mainLayout);
	resize(440, 340);
}

void SettingsDialog::globalConfigToggle(int state)
{
	sptGroup->setEnabled(state == Qt::Unchecked);
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
		QApplication::setStyle(settings.general.style);
		Utils::applyPalette(settings.general.stylePalette);
	}

	// Media controller
	if (appMedia != nullptr)
	{
		if (appMedia->isChecked() != settings.general.mediaController)
			QMessageBox::information(
				this, "Media Controller",
				"Please restart the application to apply changes");
		settings.general.mediaController = appMedia->isChecked();
	}

	// PulseAudio volume
	if (appPulse != nullptr)
		settings.general.pulseVolume = appPulse->isChecked();

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

	// Desktop notifications and tray icon
	if (itfTrayNotify->isChecked() && !itfTrayIcon->isChecked())
	{
		itfTrayIcon->setChecked(true);
		QMessageBox::information(
			this, "Desktop Notifications",
			"Desktop notifications requires tray icon to be enabled, so it was enabled");
	}
	// Check if tray icon needs to be reloaded
	auto reloadTray = settings.general.trayIcon != itfTrayIcon->isChecked()
		|| settings.general.trayNotifications != itfTrayNotify->isChecked()
		|| settings.general.trayLightIcon != itfTrayInvert->isChecked();
	// Apply
	settings.general.trayIcon = itfTrayIcon->isChecked();
	settings.general.trayNotifications = itfTrayNotify->isChecked();
	settings.general.trayLightIcon = itfTrayInvert->isChecked();
	settings.general.trayAlbumArt = itfTrayAlbum->isChecked();
	// Reload if needed
	auto window = dynamic_cast<MainWindow*>(parent());
	if (reloadTray && window != nullptr)
		window->reloadTrayIcon();
	// Song header resize mode
	auto resizeMode = itfResizeAuto->isChecked()
		? QHeaderView::ResizeToContents
		: QHeaderView::Interactive;
	if (resizeMode != settings.general.songHeaderResizeMode && window != nullptr)
		window->getSongsTree()->header()->setSectionResizeMode(resizeMode);
	settings.general.songHeaderResizeMode = resizeMode;

	// Refresh interval
	auto ok = false;
	auto interval = appRefresh->currentText().toInt(&ok);
	if (!ok || interval <= 0 || interval > 60)
	{
		applyFail("refresh interval");
		return false;
	}
	settings.general.refreshInterval = interval;

	// Spotify global config
	if (sptGlobal->isChecked() && !sptConfigExists())
		QMessageBox::warning(this,
			"spotifyd config not found",
			QString("Couldn't find a config file for spotifyd. You may experience issues."));
	settings.spotify.globalConfig = sptGlobal->isChecked();

	// Other application stuff
	settings.general.showChangelog = appWhatsNew->isChecked();
	settings.general.spotifyPlaybackOrder = appSptOrder->isChecked();
	settings.general.singleClickPlay = appOneClick->isChecked();

	// Other interface stuff
	if (itfIcFallback != nullptr)
		settings.general.fallbackIcons = itfIcFallback->isChecked();
	if (window != nullptr)
		window->setFixedWidthTime(itfMonoTime->isChecked());
	settings.general.fixedWidthTime = itfMonoTime->isChecked();
	settings.general.showContextInfo = itfContextInfo->isChecked();

	// Other Spotify stuff
	settings.spotify.startClient =sptAppStart->isChecked();
	settings.spotify.username = sptUsername->text();
	auto bitrate = sptBitrate->currentIndex();
	settings.spotify.bitrate = bitrate == 0 ? 96 : bitrate == 1 ? 160 : 320;
	settings.spotify.alwaysStart = sptAlways->isChecked();

	// Playlist stuff
	auto playlistOrder = (PlaylistOrder) plOrder->currentIndex();
	if (settings.general.playlistOrder != playlistOrder && window != nullptr)
		window->orderPlaylists(playlistOrder);
	settings.general.playlistOrder = playlistOrder;

	// Everything is fine
	settings.save();
	return true;
}

void SettingsDialog::applyFail(const QString &setting)
{
	QMessageBox::warning(this,
		"Failed to apply settings",
		QString("Failed to apply setting \"%1\". Check your settings and try again.").arg(setting));
}

bool SettingsDialog::sptConfigExists()
{
	// Config is either ~/.config/spotifyd/spotifyd.conf or /etc/spotifyd/spotifyd.conf
	return QFile(QString("%1/.config/spotifyd/spotifyd.conf")
		.arg(QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0])).exists()
		|| QFile("/etc/spotifyd/spotifyd.conf").exists();
}