#include "interfacepage.hpp"

InterfacePage::InterfacePage(lib::settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	addTab(general(), "General");
	addTab(trayIcon(), "Tray icon");
}

QWidget *InterfacePage::general()
{
	auto layout = tabContent();
	auto comboBoxLayout = new QGridLayout();

	// Style
	auto styleLabel = new QLabel("Style", this);
	styleLabel->setToolTip("Qt style to use "
						   "(Fusion is recommended when using the custom dark theme)");
	comboBoxLayout->addWidget(styleLabel, 0, 0);

	auto styles = QStyleFactory::keys();

	itfStyle = new QComboBox(this);
	itfStyle->addItem("Default");
	itfStyle->addItems(styles);
	comboBoxLayout->addWidget(itfStyle, 0, 1);

	if (styles.contains(QString::fromStdString(settings.general.style)))
		itfStyle->setCurrentText(QString::fromStdString(settings.general.style));
	else if (styles.contains(QApplication::style()->objectName()))
		itfStyle->setCurrentText(QApplication::style()->objectName());
	else
		itfStyle->setCurrentIndex(0);

	layout->addLayout(comboBoxLayout);

	// Dark theme
	itfDark = new QCheckBox("Dark theme", this);
	itfDark->setToolTip("Use custom dark theme");
	itfDark->setChecked(settings.get_dark_theme());
	QCheckBox::connect(itfDark, &QCheckBox::toggled, this, &InterfacePage::darkThemeToggle);
	layout->addWidget(itfDark);

	// Song header resize mode
	itfResizeAuto = new QCheckBox("Auto resize track list headers", this);
	itfResizeAuto->setToolTip("Automatically resize track list headers to fit content");
	itfResizeAuto->setChecked(settings.general
		.song_header_resize_mode == QHeaderView::ResizeToContents);
	layout->addWidget(itfResizeAuto);

	// Always use fallback icons (if system icons are an option)
	if (hasIconTheme())
	{
		itfIcFallback = new QCheckBox("Always use built-in icons", this);
		itfIcFallback->setToolTip("Always use bundled icons, even if system icons are available");
		itfIcFallback->setChecked(settings.general.fallback_icons);
		layout->addWidget(itfIcFallback);
	}

	// Monospace remaining time
	itfMonoTime = new QCheckBox("Fixed width remaining time", this);
	itfMonoTime->setToolTip("Use a fixed width for remaining time to avoid resizing");
	itfMonoTime->setChecked(settings.general.fixed_width_time);
	layout->addWidget(itfMonoTime);

	// Context info
	itfContextInfo = new QCheckBox("Show context information", this);
	itfContextInfo->setToolTip("Show what context music is currently playing from "
							   "above current track");
	itfContextInfo->setChecked(settings.general.show_context_info);
	layout->addWidget(itfContextInfo);

	// Track numbers
	itfTrackNum = new QCheckBox("Show track numbers", this);
	itfTrackNum->setToolTip("Show track numbers next to tracks in the list");
	itfTrackNum->setChecked(settings.general.track_numbers == lib::context_all);
	layout->addWidget(itfTrackNum);

	// Relative added date
	itfRelativeAdded = new QCheckBox("Relative added dates", this);
	itfRelativeAdded->setToolTip("Relative added dates compared to current date, "
								 "for example \"... ago\"");
	itfRelativeAdded->setChecked(settings.general.relative_added);
	layout->addWidget(itfRelativeAdded);

	return Utils::layoutToWidget(layout);
}

QWidget *InterfacePage::trayIcon()
{
	// Main container for everything
	auto content = new QVBoxLayout();
	content->setAlignment(Qt::AlignTop);

	// Tray icon settings
	itfTrayIcon = new QCheckBox("Enabled", this);
	itfTrayIcon->setToolTip("Add an icon to the system tray for quick access");
	itfTrayIcon->setChecked(settings.general.tray_icon);
	content->addWidget(itfTrayIcon);

	// Desktop notifications
	itfTrayNotify = new QCheckBox("Desktop notifications", this);
	itfTrayNotify->setToolTip("Replace status bar with desktop notifications "
							  "(suppresses any non-error messages)");
	itfTrayNotify->setChecked(settings.general.tray_notifications);
	content->addWidget(itfTrayNotify);
	QCheckBox::connect(itfTrayNotify, &QCheckBox::stateChanged, [this](int state)
	{
		if (state == Qt::CheckState::Checked && itfTrayIcon != nullptr)
			itfTrayIcon->setChecked(true);
	});

	// Invert tray icon
	itfTrayInvert = new QCheckBox("Invert icon", this);
	itfTrayInvert->setToolTip("Invert colors in tray icon to be visible on light backgrounds");
	itfTrayInvert->setChecked(settings.general.tray_light_icon);
	content->addWidget(itfTrayInvert);

	// Album art in tray
	itfTrayAlbum = new QCheckBox("Album art as icon", this);
	itfTrayAlbum->setToolTip("Show album art of current track in tray icon");
	itfTrayAlbum->setChecked(settings.general.tray_album_art);
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
	auto mainWindow = MainWindow::find(parentWidget());

	// Set theme
	if (itfDark->isChecked() != settings.get_dark_theme())
	{
		QMessageBox::information(this, "Dark Theme",
			"Please restart the application to fully apply selected theme");
		settings.set_dark_theme(itfDark->isChecked());
		Utils::applyPalette(settings.general.style_palette);
	}

	// Set style
	if (itfStyle->currentText() != QString::fromStdString(settings.general.style))
	{
		QApplication::setStyle(itfStyle->currentText());
		settings.general.style = itfStyle->currentText().toStdString();
	}

	// Song header resize mode
	auto resizeMode = itfResizeAuto->isChecked()
		? QHeaderView::ResizeToContents
		: QHeaderView::Interactive;
	if (resizeMode != settings.general.song_header_resize_mode && mainWindow != nullptr)
		mainWindow->getSongsTree()->header()->setSectionResizeMode(resizeMode);
	settings.general.song_header_resize_mode = resizeMode;

	// Track numbers
	if (mainWindow != nullptr)
		mainWindow->toggleTrackNumbers(itfTrackNum->isChecked());
	settings.general.track_numbers = itfTrackNum->isChecked()
		? lib::context_all
		: lib::context_none;

	// Other interface stuff
	if (itfIcFallback != nullptr)
		settings.general.fallback_icons = itfIcFallback->isChecked();
	if (mainWindow != nullptr)
		mainWindow->setFixedWidthTime(itfMonoTime->isChecked());
	settings.general.fixed_width_time = itfMonoTime->isChecked();
	settings.general.show_context_info = itfContextInfo->isChecked();
	settings.general.relative_added = itfRelativeAdded->isChecked();

	// Desktop notifications and tray icon
	if (itfTrayNotify->isChecked() && !itfTrayIcon->isChecked())
	{
		itfTrayIcon->setChecked(true);
		QMessageBox::information(this, "Desktop Notifications",
			"Desktop notifications requires tray icon to be enabled, so it was enabled");
	}

	// Check if tray icon needs to be reloaded
	auto reloadTray = settings.general.tray_icon != itfTrayIcon->isChecked()
		|| settings.general.tray_notifications != itfTrayNotify->isChecked()
		|| settings.general.tray_light_icon != itfTrayInvert->isChecked();

	// Apply
	settings.general.tray_icon = itfTrayIcon->isChecked();
	settings.general.tray_notifications = itfTrayNotify->isChecked();
	settings.general.tray_light_icon = itfTrayInvert->isChecked();
	settings.general.tray_album_art = itfTrayAlbum->isChecked();

	// Reload if needed
	if (reloadTray && mainWindow != nullptr)
		mainWindow->reloadTrayIcon();

	return true;
}

bool InterfacePage::hasIconTheme()
{
	return !QIcon::fromTheme("media-playback-start").isNull();
}

void InterfacePage::darkThemeToggle(bool checked)
{
	if (checked)
	{
		itfStyle->setCurrentText("Fusion");
		itfIcFallback->setChecked(true);
	}
	else
	{
		itfStyle->setCurrentIndex(0);
	}
}
