#include "interfacepage.hpp"

InterfacePage::InterfacePage(Settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	tabs = new QTabWidget(this);
	tabs->addTab(interface(), "General");
	tabs->addTab(trayIcon(), "Tray icon");
	layout->addWidget(tabs);
}

QWidget *InterfacePage::interface()
{
	auto content = new QVBoxLayout();
	content->setAlignment(Qt::AlignTop);

	// Dark theme
	itfDark = new QCheckBox("Dark theme", this);
	itfDark->setToolTip("Use custom dark theme");
	itfDark->setChecked(settings.darkTheme());
	content->addWidget(itfDark);

	// Song header resize mode
	itfResizeAuto = new QCheckBox("Auto resize track list headers", this);
	itfResizeAuto->setToolTip("Automatically resize track list headers to fit content");
	itfResizeAuto->setChecked(settings.general.songHeaderResizeMode == QHeaderView::ResizeToContents);
	content->addWidget(itfResizeAuto);

	// Always use fallback icons (if system icons are an option)
	if (hasIconTheme())
	{
		itfIcFallback = new QCheckBox("Always use fallback icons", this);
		itfIcFallback->setToolTip("Always use bundled fallback icons, even if system icons are available");
		itfIcFallback->setChecked(settings.general.fallbackIcons);
		content->addWidget(itfIcFallback);
	}

	// Monospace remaining time
	itfMonoTime = new QCheckBox("Fixed width remaining time", this);
	itfMonoTime->setToolTip("Use a fixed width for remaining time to avoid resizing");
	itfMonoTime->setChecked(settings.general.fixedWidthTime);
	content->addWidget(itfMonoTime);

	// Context info
	itfContextInfo = new QCheckBox("Show context information", this);
	itfContextInfo->setToolTip("Show what context music is currently playing from above current track");
	itfContextInfo->setChecked(settings.general.showContextInfo);
	content->addWidget(itfContextInfo);

	// Track numbers
	itfTrackNum = new QCheckBox("Show track numbers", this);
	itfTrackNum->setToolTip("Show track numbers next to tracks in the list");
	itfTrackNum->setChecked(settings.general.trackNumbers == ContextAll);
	content->addWidget(itfTrackNum);

	return Utils::layoutToWidget(content);
}

QWidget *InterfacePage::trayIcon()
{
	// Main container for everything
	auto content = new QVBoxLayout();
	content->setAlignment(Qt::AlignTop);

	// Tray icon settings
	itfTrayIcon = new QCheckBox("Enabled", this);
	itfTrayIcon->setToolTip("Add an icon to the system tray for quick access");
	itfTrayIcon->setChecked(settings.general.trayIcon);
	content->addWidget(itfTrayIcon);

	// Desktop notifications
	itfTrayNotify = new QCheckBox("Desktop notifications", this);
	itfTrayNotify->setToolTip("Replace status bar with desktop notifications (suppresses any non-error messages)");
	itfTrayNotify->setChecked(settings.general.trayNotifications);
	content->addWidget(itfTrayNotify);
	QCheckBox::connect(itfTrayNotify, &QCheckBox::stateChanged, [this](int state)
	{
		if (state == Qt::CheckState::Checked && itfTrayIcon != nullptr)
			itfTrayIcon->setChecked(true);
	});

	// Invert tray icon
	itfTrayInvert = new QCheckBox("Invert icon", this);
	itfTrayInvert->setToolTip("Invert colors in tray icon to be visible on light backgrounds");
	itfTrayInvert->setChecked(settings.general.trayLightIcon);
	content->addWidget(itfTrayInvert);

	// Album art in tray
	itfTrayAlbum = new QCheckBox("Album art as icon", this);
	itfTrayAlbum->setToolTip("Show album art of current track in tray icon");
	itfTrayAlbum->setChecked(settings.general.trayAlbumArt);
	content->addWidget(itfTrayAlbum);

	return Utils::layoutToWidget(content);
}

QIcon InterfacePage::icon()
{
	return Icon::get("draw-brush");
}

QString InterfacePage::title()
{
	return "Interface";
}

bool InterfacePage::save()
{
	auto mainWindow = dynamic_cast<MainWindow*>(findMainWindow());

	// Set theme
	auto changeTheme = itfDark->isChecked() != settings.darkTheme();
	if (changeTheme)
	{
		QMessageBox::information(this, "Dark Theme",
			"Please restart the application to fully apply selected theme");
		settings.setDarkTheme(itfDark->isChecked());
		QApplication::setStyle(settings.general.style);
		Utils::applyPalette(settings.general.stylePalette);
	}

	// Song header resize mode
	auto resizeMode = itfResizeAuto->isChecked()
		? QHeaderView::ResizeToContents
		: QHeaderView::Interactive;
	if (resizeMode != settings.general.songHeaderResizeMode && mainWindow != nullptr)
		mainWindow->getSongsTree()->header()->setSectionResizeMode(resizeMode);
	settings.general.songHeaderResizeMode = resizeMode;

	// Track numbers
	if (mainWindow != nullptr)
		mainWindow->toggleTrackNumbers(itfTrackNum->isChecked());
	settings.general.trackNumbers = itfTrackNum->isChecked()
		? ContextAll
		: ContextNone;

	// Other interface stuff
	if (itfIcFallback != nullptr)
		settings.general.fallbackIcons = itfIcFallback->isChecked();
	if (mainWindow != nullptr)
		mainWindow->setFixedWidthTime(itfMonoTime->isChecked());
	settings.general.fixedWidthTime = itfMonoTime->isChecked();
	settings.general.showContextInfo = itfContextInfo->isChecked();

	// Desktop notifications and tray icon
	if (itfTrayNotify->isChecked() && !itfTrayIcon->isChecked())
	{
		itfTrayIcon->setChecked(true);
		QMessageBox::information(this, "Desktop Notifications",
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
	auto window = dynamic_cast<MainWindow *>(parent());
	if (reloadTray && window != nullptr)
		window->reloadTrayIcon();

	return false;
}

bool InterfacePage::hasIconTheme()
{
	return !QIcon::fromTheme("media-playback-start").isNull();
}
