#include "interfacepage.hpp"
#include "mainwindow.hpp"

InterfacePage::InterfacePage(lib::settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	addTab(general(), "General");
	addTab(appearance(), "Appearance");
	addTab(trayIcon(), "Tray icon");
}

auto InterfacePage::general() -> QWidget *
{
	auto *layout = tabContent();
	auto *comboBoxLayout = new QGridLayout();

	// Column resize
	auto *resizeLabel = new QLabel("Track column resize", this);
	resizeLabel->setToolTip("How to resize the columns in the track list\n"
							"Fit width (default): Adjust column width to fill width of list\n"
							"Fit content: Adjust column width to show all content\n"
							"Custom: No auto resize");
	comboBoxLayout->addWidget(resizeLabel, 0, 0);

	itfResizeMode = new QComboBox(this);
	itfResizeMode->addItems({
		"Fit width",
		"Fit content",
		"Custom",
	});
	itfResizeMode->setCurrentIndex(static_cast<int>(settings.general.track_list_resize_mode));
	comboBoxLayout->addWidget(itfResizeMode, 0, 1);
	layout->addLayout(comboBoxLayout);

	// Monospace remaining time
	itfMonoTime = new QCheckBox("Fixed width remaining time", this);
	itfMonoTime->setToolTip("Use a fixed width for remaining time to avoid resizing");
	itfMonoTime->setChecked(settings.general.fixed_width_time);
	layout->addWidget(itfMonoTime);

	// Track numbers
	itfTrackNum = new QCheckBox("Show track numbers", this);
	itfTrackNum->setToolTip("Show track numbers next to tracks in the list");
	itfTrackNum->setChecked(settings.general.track_numbers == lib::spotify_context::all);
	layout->addWidget(itfTrackNum);

	// Relative added date
	itfRelativeAdded = new QCheckBox("Relative added dates", this);
	itfRelativeAdded->setToolTip("Relative added dates compared to current date,\n"
								 "for example \"... ago\"");
	itfRelativeAdded->setChecked(settings.general.relative_added);
	layout->addWidget(itfRelativeAdded);

	// System title bar
	const auto &qtSettings = settings.qt_const();
	titleBar = new QCheckBox("System title bar", this);
	titleBar->setToolTip("Show system title bar and window borders");
	titleBar->setChecked(qtSettings.system_title_bar);
	layout->addWidget(titleBar);

	return WidgetUtils::layoutToWidget(layout, this);
}

auto InterfacePage::appearance() -> QWidget *
{
	auto *layout = tabContent();
	auto *comboBoxLayout = new QGridLayout();

	const auto &qt = settings.qt_const();

	// Style
	auto *styleLabel = new QLabel("Style", this);
	styleLabel->setToolTip("Qt style to use\n"
						   "(Fusion is recommended when using the custom dark theme)");
	comboBoxLayout->addWidget(styleLabel, 0, 0);

	auto styles = QStyleFactory::keys();

	itfStyle = new QComboBox(this);
	itfStyle->addItem("Default");
	itfStyle->addItems(styles);
	comboBoxLayout->addWidget(itfStyle, 0, 1);

	if (styles.contains(QString::fromStdString(settings.general.style)))
	{
		itfStyle->setCurrentText(QString::fromStdString(settings.general.style));
	}
	else if (styles.contains(QApplication::style()->objectName()))
	{
		itfStyle->setCurrentText(QApplication::style()->objectName());
	}
	else
	{
		itfStyle->setCurrentIndex(0);
	}

	layout->addLayout(comboBoxLayout);

	// Dark theme
	itfDark = new QCheckBox("Dark theme", this);
	itfDark->setToolTip("Use custom dark theme");
	itfDark->setChecked(settings.get_dark_theme());
	QCheckBox::connect(itfDark, &QCheckBox::toggled, this, &InterfacePage::darkThemeToggle);
	layout->addWidget(itfDark);

	// Always use fallback icons (if system icons are an option)
	if (hasIconTheme())
	{
		itfIcFallback = new QCheckBox("Use bundled icons", this);
		itfIcFallback->setToolTip("Always use bundled icons, even if system icons are available");
		itfIcFallback->setChecked(settings.general.fallback_icons);
		layout->addWidget(itfIcFallback);
	}

	// Custom font
	itfFont = new QCheckBox(QStringLiteral("Custom font"), this);
	itfFont->setToolTip(QStringLiteral("Use custom Noto Sans font"));
	itfFont->setChecked(qt.custom_font);
	layout->addWidget(itfFont);

	return WidgetUtils::layoutToWidget(layout, this);
}

auto InterfacePage::trayIcon() -> QWidget *
{
	// Main container for everything
	auto *content = new QVBoxLayout();
	content->setAlignment(Qt::AlignTop);

	// Tray icon settings
	itfTrayIcon = new QCheckBox("Enabled", this);
	itfTrayIcon->setToolTip("Add an icon to the system tray for quick access");
	itfTrayIcon->setChecked(settings.general.tray_icon);
	content->addWidget(itfTrayIcon);

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

	return WidgetUtils::layoutToWidget(content, this);
}

auto InterfacePage::icon() -> QIcon
{
	return Icon::get("draw-brush");
}

auto InterfacePage::title() -> QString
{
	return "Interface";
}

auto InterfacePage::save() -> bool
{
	auto *mainWindow = MainWindow::find(parentWidget());

	// Set theme
	if (itfDark != nullptr
		&& itfDark->isChecked() != settings.get_dark_theme())
	{
		QMessageBox::information(this, "Dark Theme",
			"Please restart the application to fully apply selected theme");
		settings.set_dark_theme(itfDark->isChecked());
		StyleUtils::applyPalette(settings.general.style_palette);
	}

	// Set style
	if (itfStyle != nullptr
		&& itfStyle->currentText() != QString::fromStdString(settings.general.style))
	{
		QApplication::setStyle(itfStyle->currentIndex() == 0
			? defaultStyle()
			: itfStyle->currentText());

		settings.general.style = itfStyle->currentIndex() == 0
			? std::string()
			: itfStyle->currentText().toStdString();
	}

	// Track list resize mode
	if (itfResizeMode != nullptr)
	{
		auto resizeMode = static_cast<lib::resize_mode>(itfResizeMode->currentIndex());
		if (mainWindow != nullptr
			&& settings.general.track_list_resize_mode != resizeMode)
		{
			auto *tracksList = dynamic_cast<TracksList *>(mainWindow->getSongsTree());
			if (tracksList != nullptr)
			{
				tracksList->updateResizeMode(resizeMode);
			}
		}
		settings.general.track_list_resize_mode = resizeMode;
	}

	// Track numbers
	if (itfTrackNum != nullptr)
	{
		if (mainWindow != nullptr)
		{
			mainWindow->toggleTrackNumbers(itfTrackNum->isChecked());
		}
		settings.general.track_numbers = itfTrackNum->isChecked()
			? lib::spotify_context::all
			: lib::spotify_context::none;
	}

	// Other interface stuff
	if (itfIcFallback != nullptr)
	{
		settings.general.fallback_icons = itfIcFallback->isChecked();
	}

	if (itfMonoTime != nullptr)
	{
		if (mainWindow != nullptr)
		{
			mainWindow->setFixedWidthTime(itfMonoTime->isChecked());
		}
		settings.general.fixed_width_time = itfMonoTime->isChecked();
	}

	if (itfRelativeAdded != nullptr)
	{
		settings.general.relative_added = itfRelativeAdded->isChecked();
	}

	// Check if tray icon needs to be reloaded
	auto reloadTray = itfTrayIcon != nullptr
		&& itfTrayInvert != nullptr
		&& (settings.general.tray_icon != itfTrayIcon->isChecked()
			|| settings.general.tray_light_icon != itfTrayInvert->isChecked());

	// Apply
	if (itfTrayIcon != nullptr)
	{
		settings.general.tray_icon = itfTrayIcon->isChecked();
	}
	if (itfTrayInvert != nullptr)
	{
		settings.general.tray_light_icon = itfTrayInvert->isChecked();
	}
	if (itfTrayAlbum != nullptr)
	{
		settings.general.tray_album_art = itfTrayAlbum->isChecked();
	}

	// Qt specific
	auto &qtSettings = settings.qt();
	if (titleBar != nullptr)
	{
		if (qtSettings.system_title_bar != titleBar->isChecked())
		{
			info(titleBar->text(),
				"Please restart the application to toggle system title bar");
		}
		qtSettings.system_title_bar = titleBar->isChecked();
	}
	if (itfFont != nullptr)
	{
		if (qtSettings.custom_font != itfFont->isChecked())
		{
			info(itfFont->text(), "Please restart the application to change font");
		}

		qtSettings.custom_font = itfFont->isChecked();
	}

	// Reload if needed
	if (reloadTray && mainWindow != nullptr)
	{
		mainWindow->reloadTrayIcon();
	}

	return true;
}

auto InterfacePage::hasIconTheme() -> bool
{
	return !QIcon::fromTheme("media-playback-start").isNull();
}

void InterfacePage::darkThemeToggle(bool checked)
{
	if (itfStyle == nullptr)
	{
		return;
	}

	if (checked)
	{
		itfStyle->setCurrentText("Fusion");

		if (itfIcFallback != nullptr)
		{
			itfIcFallback->setChecked(true);
		}
	}
	else
	{
		itfStyle->setCurrentIndex(0);
	}

	if (itfFont != nullptr)
	{
		itfFont->setChecked(checked);
	}
}

auto InterfacePage::defaultStyle() -> QString
{
	// Find default style on KDE
	const auto &path = QStandardPaths::locate(QStandardPaths::GenericConfigLocation,
		"kdeglobals");
	if (QFileInfo::exists(path))
	{
		QSettings settings(path, QSettings::IniFormat);
		auto style = settings.value("KDE/widgetStyle").toString();
		if (!style.isEmpty())
		{
			return style;
		}
	}

	// Override from environmental variable
	auto overridden = lib::system::env("QT_STYLE_OVERRIDE");
	if (!overridden.empty())
	{
		return QString::fromStdString(overridden);
	}

	// Assume Fusion
	return QString("Fusion");
}
