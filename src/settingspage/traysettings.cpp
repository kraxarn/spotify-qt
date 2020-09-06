#include "traysettings.hpp"

TraySettings::TraySettings(Settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	// Tray icon settings
	itfTrayIcon = new QCheckBox("Enabled", this);
	itfTrayIcon->setToolTip("Add an icon to the system tray for quick access");
	itfTrayIcon->setChecked(settings.general.trayIcon);
	page->addWidget(itfTrayIcon);

	// Desktop notifications
	itfTrayNotify = new QCheckBox("Desktop notifications", this);
	itfTrayNotify->setToolTip("Replace status bar with desktop notifications (suppresses any non-error messages)");
	itfTrayNotify->setChecked(settings.general.trayNotifications);
	page->addWidget(itfTrayNotify);
	QCheckBox::connect(itfTrayNotify, &QCheckBox::stateChanged, [this](int state)
	{
		if (state == Qt::CheckState::Checked && itfTrayIcon != nullptr)
		{
			itfTrayIcon->setChecked(true);
		}
	});

	// Invert tray icon
	itfTrayInvert = new QCheckBox("Invert icon", this);
	itfTrayInvert->setToolTip("Invert colors in tray icon to be visible on light backgrounds");
	itfTrayInvert->setChecked(settings.general.trayLightIcon);
	page->addWidget(itfTrayInvert);

	// Album art in tray
	itfTrayAlbum = new QCheckBox("Album art as icon", this);
	itfTrayAlbum->setToolTip("Show album art of current track in tray icon");
	itfTrayAlbum->setChecked(settings.general.trayAlbumArt);
	page->addWidget(itfTrayAlbum);
}

QString TraySettings::title()
{
	return "Tray icon";
}

QWidget *TraySettings::toWidget()
{
	return this;
}

bool TraySettings::applySettings(QWidget *mainWindow)
{
	auto window = dynamic_cast<MainWindow *>(mainWindow);

	// Desktop notifications and tray icon
	if (itfTrayNotify->isChecked() && !itfTrayIcon->isChecked())
	{
		itfTrayIcon->setChecked(true);
		QMessageBox::information(
			this,
			"Desktop Notifications",
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
	if (reloadTray && window != nullptr)
		window->reloadTrayIcon();

	return true;
}