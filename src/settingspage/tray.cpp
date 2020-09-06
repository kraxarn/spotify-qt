#include "../dialog/settingsdialog.hpp"

QWidget *SettingsDialog::traySettings()
{
	// Main container for everything
	auto layout = new QVBoxLayout();
	layout->setAlignment(Qt::AlignTop);

	// Tray icon settings
	itfTrayIcon = new QCheckBox("Enabled", this);
	itfTrayIcon->setToolTip("Add an icon to the system tray for quick access");
	itfTrayIcon->setChecked(settings.general.trayIcon);
	layout->addWidget(itfTrayIcon);

	// Desktop notifications
	itfTrayNotify = new QCheckBox("Desktop notifications", this);
	itfTrayNotify->setToolTip("Replace status bar with desktop notifications (suppresses any non-error messages)");
	itfTrayNotify->setChecked(settings.general.trayNotifications);
	layout->addWidget(itfTrayNotify);
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
	layout->addWidget(itfTrayInvert);

	// Album art in tray
	itfTrayAlbum = new QCheckBox("Album art as icon", this);
	itfTrayAlbum->setToolTip("Show album art of current track in tray icon");
	itfTrayAlbum->setChecked(settings.general.trayAlbumArt);
	layout->addWidget(itfTrayAlbum);

	// Final layout
	auto widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}