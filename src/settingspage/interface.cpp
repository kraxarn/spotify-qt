#include "settingspage/interface.hpp"
#include "mainwindow.hpp"

SettingsPage::Interface::Interface(lib::settings &settings, QWidget *parent)
	: SettingsPage::Base(settings, parent)
{
	addTab(general(), "General");
	addTab(appearance(), "Appearance");
	addTab(trayIcon(), "Tray icon");
	addTab(titleBar(), "Title bar");
}

auto SettingsPage::Interface::general() -> QWidget *
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

	resizeMode = new QComboBox(this);
	resizeMode->addItems({
		"Fit width",
		"Fit content",
		"Custom",
	});
	resizeMode->setCurrentIndex(static_cast<int>(settings.general.track_list_resize_mode));
	comboBoxLayout->addWidget(resizeMode, 0, 1);
	layout->addLayout(comboBoxLayout);

	// Monospace remaining time
	monoTime = new QCheckBox("Fixed width remaining time", this);
	monoTime->setToolTip("Use a fixed width for remaining time to avoid resizing");
	monoTime->setChecked(settings.general.fixed_width_time);
	layout->addWidget(monoTime);

	// Track numbers
	trackNumbers = new QCheckBox("Show track numbers", this);
	trackNumbers->setToolTip("Show track numbers next to tracks in the list");
	trackNumbers->setChecked(settings.general.track_numbers == lib::spotify_context::all);
	layout->addWidget(trackNumbers);

	// Relative added date
	relativeAdded = new QCheckBox("Relative added dates", this);
	relativeAdded->setToolTip("Relative added dates compared to current date,\n"
							  "for example \"... ago\"");
	relativeAdded->setChecked(settings.general.relative_added);
	layout->addWidget(relativeAdded);

	return Widget::layoutToWidget(layout, this);
}

auto SettingsPage::Interface::appearance() -> QWidget *
{
	auto *layout = tabContent();
	auto *comboBoxLayout = new QGridLayout();

	const auto &qt = settings.qt();

	// Style
	auto *styleLabel = new QLabel("Style", this);
	styleLabel->setToolTip("Qt style to use\n"
						   "(Fusion is recommended when using the custom dark theme)");
	comboBoxLayout->addWidget(styleLabel, 0, 0);

	auto styles = QStyleFactory::keys();

	qtStyle = new QComboBox(this);
	qtStyle->addItem("Default");
	qtStyle->addItems(styles);
	comboBoxLayout->addWidget(qtStyle, 0, 1);

	if (styles.contains(QString::fromStdString(settings.general.style)))
	{
		qtStyle->setCurrentText(QString::fromStdString(settings.general.style));
	}
	else if (styles.contains(QApplication::style()->objectName()))
	{
		qtStyle->setCurrentText(QApplication::style()->objectName());
	}
	else
	{
		qtStyle->setCurrentIndex(0);
	}

	layout->addLayout(comboBoxLayout);

	// Dark theme
	darkTheme = new QCheckBox("Dark theme", this);
	darkTheme->setToolTip("Use custom dark theme");
	darkTheme->setChecked(settings.get_dark_theme());
	QCheckBox::connect(darkTheme, &QCheckBox::toggled,
		this, &SettingsPage::Interface::darkThemeToggle);
	layout->addWidget(darkTheme);

	// Always use fallback icons (if system icons are an option)
	if (hasIconTheme())
	{
		iconFallback = new QCheckBox("Use bundled icons", this);
		iconFallback->setToolTip("Always use bundled icons, even if system icons are available");
		iconFallback->setChecked(settings.general.fallback_icons);
		layout->addWidget(iconFallback);
	}

	// Custom font
	customFont = new QCheckBox(QStringLiteral("Custom font"), this);
	customFont->setToolTip(QStringLiteral("Use custom Noto Sans font"));
	customFont->setChecked(qt.custom_font);
	layout->addWidget(customFont);

	return Widget::layoutToWidget(layout, this);
}

auto SettingsPage::Interface::trayIcon() -> QWidget *
{
	// Main container for everything
	auto *content = new QVBoxLayout();
	content->setAlignment(Qt::AlignTop);

	// Tray icon settings
	trayEnabled = new QGroupBox("Enabled", this);
	trayEnabled->setCheckable(true);
	trayEnabled->setToolTip("Add an icon to the system tray for quick access");
	trayEnabled->setChecked(settings.general.tray_icon);
	content->addWidget(trayEnabled);

	// Container for options
	auto *trayOptions = new QVBoxLayout();
	trayEnabled->setLayout(trayOptions);

	// Invert tray icon
	invertTrayIcon = new QCheckBox("Invert icon", this);
	invertTrayIcon->setToolTip("Invert colors in tray icon to be visible on light backgrounds");
	invertTrayIcon->setChecked(settings.general.tray_light_icon);
	trayOptions->addWidget(invertTrayIcon);

	// Album art in tray
	albumInTray = new QCheckBox("Album art as icon", this);
	albumInTray->setToolTip("Show album art of current track in tray icon");
	albumInTray->setChecked(settings.general.tray_album_art);
	trayOptions->addWidget(albumInTray);

	// Notify on track change
	notifyTrackChange = new QCheckBox("Show notification on track change", this);
	notifyTrackChange->setToolTip("Show desktop notification when a new track starts playing");
	notifyTrackChange->setChecked(settings.general.notify_track_change);
	trayOptions->addWidget(notifyTrackChange);

	return Widget::layoutToWidget(content, this);
}

auto SettingsPage::Interface::titleBar() -> QWidget *
{
	const auto &qt = settings.qt();

	auto *content = new QVBoxLayout();
	content->setAlignment(Qt::AlignTop);

	// System title bar settings
	appTitleBar = new QGroupBox("Application title bar", this);
	appTitleBar->setToolTip("Embed custom title bar in application");
	appTitleBar->setCheckable(true);
	appTitleBar->setChecked(!qt.system_title_bar);
	content->addWidget(appTitleBar);

	// Container for options
	auto *titleBarOptions = new QVBoxLayout();
	appTitleBar->setLayout(titleBarOptions);

	mirrorTitleBar = new QCheckBox("Mirror buttons", this);
	mirrorTitleBar->setToolTip("Mirror buttons and show close/minimize on the left side");
	mirrorTitleBar->setChecked(qt.mirror_title_bar);
	titleBarOptions->addWidget(mirrorTitleBar);

	return Widget::layoutToWidget(content, this);
}

auto SettingsPage::Interface::icon() -> QIcon
{
	return Icon::get("draw-brush");
}

auto SettingsPage::Interface::title() -> QString
{
	return "Interface";
}

void SettingsPage::Interface::saveGeneral()
{
	auto *mainWindow = MainWindow::find(parentWidget());

	if (resizeMode != nullptr)
	{
		auto newResizeMode = static_cast<lib::resize_mode>(resizeMode->currentIndex());

		if (mainWindow != nullptr
			&& settings.general.track_list_resize_mode != newResizeMode)
		{
			auto *tracksList = mainWindow->getSongsTree();
			if (tracksList != nullptr)
			{
				tracksList->updateResizeMode(newResizeMode);
			}
		}

		settings.general.track_list_resize_mode = newResizeMode;
	}

	if (monoTime != nullptr)
	{
		if (mainWindow != nullptr)
		{
			mainWindow->setFixedWidthTime(monoTime->isChecked());
		}

		settings.general.fixed_width_time = monoTime->isChecked();
	}

	if (trackNumbers != nullptr)
	{
		if (mainWindow != nullptr)
		{
			mainWindow->toggleTrackNumbers(trackNumbers->isChecked());
		}

		settings.general.track_numbers = trackNumbers->isChecked()
			? lib::spotify_context::all
			: lib::spotify_context::none;
	}

	if (relativeAdded != nullptr)
	{
		settings.general.relative_added = relativeAdded->isChecked();
	}
}

void SettingsPage::Interface::saveAppearance()
{
	if (qtStyle != nullptr
		&& qtStyle->currentText() != QString::fromStdString(settings.general.style))
	{
		QApplication::setStyle(qtStyle->currentIndex() == 0
			? defaultStyle()
			: qtStyle->currentText());

		settings.general.style = qtStyle->currentIndex() == 0
			? std::string()
			: qtStyle->currentText().toStdString();
	}

	if (darkTheme != nullptr
		&& darkTheme->isChecked() != settings.get_dark_theme())
	{
		info("Dark Theme",
			"Please restart the application to fully apply selected theme");

		settings.set_dark_theme(darkTheme->isChecked());
		Style::applyPalette(settings.general.style_palette);
	}

	if (iconFallback != nullptr)
	{
		settings.general.fallback_icons = iconFallback->isChecked();
	}

	if (customFont != nullptr)
	{
		auto &qtSettings = settings.qt();

		if (qtSettings.custom_font != customFont->isChecked())
		{
			info(customFont->text(), "Please restart the application to change font");
		}

		qtSettings.custom_font = customFont->isChecked();
	}
}

void SettingsPage::Interface::saveTrayIcon()
{
	// Check if tray icon needs to be reloaded
	auto reloadTray = trayEnabled != nullptr
		&& invertTrayIcon != nullptr
		&& (settings.general.tray_icon != trayEnabled->isChecked()
			|| settings.general.tray_light_icon != invertTrayIcon->isChecked());

	if (trayEnabled != nullptr)
	{
		settings.general.tray_icon = trayEnabled->isChecked();
	}

	if (albumInTray != nullptr)
	{
		settings.general.tray_album_art = albumInTray->isChecked();
	}

	if (invertTrayIcon != nullptr)
	{
		settings.general.tray_light_icon = invertTrayIcon->isChecked();
	}

	if (notifyTrackChange != nullptr)
	{
		settings.general.notify_track_change = notifyTrackChange->isChecked();
	}

	// Reload if needed
	if (reloadTray)
	{
		auto *mainWindow = MainWindow::find(parentWidget());
		if (mainWindow != nullptr)
		{
			mainWindow->reloadTrayIcon();
		}
	}
}

void SettingsPage::Interface::saveTitleBar()
{
	auto &qt = settings.qt();

	if ((appTitleBar != nullptr && qt.system_title_bar == appTitleBar->isChecked())
		|| (mirrorTitleBar != nullptr && qt.mirror_title_bar != mirrorTitleBar->isChecked()))
	{
		info("Title bar", "Please restart the application to reload title bar");
	}

	if (appTitleBar != nullptr)
	{
		qt.system_title_bar = !appTitleBar->isChecked();
	}

	if (mirrorTitleBar != nullptr)
	{
		qt.mirror_title_bar = mirrorTitleBar->isChecked();
	}
}

auto SettingsPage::Interface::save() -> bool
{
	saveGeneral();
	saveAppearance();
	saveTrayIcon();
	saveTitleBar();

	return true;
}

auto SettingsPage::Interface::hasIconTheme() -> bool
{
	return !QIcon::fromTheme("media-playback-start").isNull();
}

void SettingsPage::Interface::darkThemeToggle(bool checked)
{
	if (qtStyle == nullptr)
	{
		return;
	}

	if (checked)
	{
		qtStyle->setCurrentText("Fusion");

		if (iconFallback != nullptr)
		{
			iconFallback->setChecked(true);
		}
	}
	else
	{
		qtStyle->setCurrentIndex(0);
	}

	if (customFont != nullptr)
	{
		customFont->setChecked(checked);
	}
}

auto SettingsPage::Interface::defaultStyle() -> QString
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
	return QStringLiteral("Fusion");
}
