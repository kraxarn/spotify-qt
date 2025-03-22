#include "settingspage/interface.hpp"
#include "mainwindow.hpp"
#include "lib/system.hpp"
#include "util/widget.hpp"

#include <QFontDialog>
#include <QStandardPaths>
#include <QSettings>

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

	const auto &qtSettings = settings.qt();

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

	// Toolbar position
	auto *toolbarPositionLabel = new QLabel(QStringLiteral("Toolbar position"), this);
	toolbarPositionLabel->setToolTip(QStringLiteral("Where to position the main toolbar"));
	comboBoxLayout->addWidget(toolbarPositionLabel, 1, 0);

	toolbarPosition = new QComboBox(this);
	toolbarPosition->addItems({
		QStringLiteral("Top"),
		QStringLiteral("Bottom"),
	});
	toolbarPosition->setCurrentIndex(qtSettings.toolbar_position == lib::position::bottom ? 1 : 0);
	comboBoxLayout->addWidget(toolbarPosition, 1, 1);

	// Album shape
	auto *albumShapeLabel = new QLabel(QStringLiteral("Album art shape"), this);
	albumShapeLabel->setToolTip(QStringLiteral("Shape of album art in main window and tray icon"));
	comboBoxLayout->addWidget(albumShapeLabel, 2, 0);

	albumShape = new AlbumShapeComboBox(this);
	for (const auto shape: albumShapes())
	{
		albumShape->addAlbumShape(shape);
		if (qtSettings.album_shape == shape)
		{
			albumShape->setCurrentIndex(albumShape->count() - 1);
		}
	}
	comboBoxLayout->addWidget(albumShape, 2, 1);

	layout->addLayout(comboBoxLayout);

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

	// Expanded album cover
	expandAlbumCover = new QCheckBox(QStringLiteral("Expanded album cover"), this);
	expandAlbumCover->setToolTip(QStringLiteral(
		"Expand size of album cover to width of side panel"));
	expandAlbumCover->setChecked(qtSettings.album_size == lib::album_size::expanded);
	layout->addWidget(expandAlbumCover);

	// Library mode
	tabbedLibrary = new QCheckBox(QStringLiteral("Tabbed library layout"), this);
	tabbedLibrary->setToolTip(QStringLiteral("Show library and playlists as tabs"));
	tabbedLibrary->setChecked(qtSettings.library_layout == lib::library_layout::tabbed);
	layout->addWidget(tabbedLibrary);

	// Native window handle
	// (Required to move window under Wayland)
	if (QGuiApplication::platformName() != "wayland")
	{
		nativeWindow = new QCheckBox(QStringLiteral("Use native window"), this);
		nativeWindow->setToolTip(QStringLiteral("Create main window with a native handle"));
		nativeWindow->setChecked(settings.general.native_window);
		layout->addWidget(nativeWindow);
	}

	return Widget::layoutToWidget(layout, this);
}

auto SettingsPage::Interface::appearance() -> QWidget *
{
	auto *layout = tabContent();
	auto *comboBoxLayout = new QGridLayout();

	const auto &qtSettings = settings.qt();

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

	// Custom font
	auto *fontLayout = new QHBoxLayout();
	layout->addLayout(fontLayout);

	auto *fontLabel = new QLabel(QStringLiteral("Font"), this);
	fontLayout->addWidget(fontLabel, 1);

	currentFont = new QLabel(this);
	fontLayout->addWidget(currentFont);

	if (qtSettings.custom_font_name.empty() && qtSettings.custom_font_size <= 0)
	{
		currentFont->setText(getDefaultFontName());
	}
	else
	{
		const auto family = QString::fromStdString(qtSettings.custom_font_name);
		currentFont->setText(getFontName(family, qtSettings.custom_font_size));
	}

	auto *selectFont = new QToolButton(this);
	selectFont->setIcon(Icon::get("document-edit"));
	selectFont->setToolTip(QStringLiteral("Select font"));
	fontLayout->addWidget(selectFont);

	QToolButton::connect(selectFont, &QAbstractButton::clicked,
		this, &SettingsPage::Interface::onSelectFont);

	auto *resetFont = new QToolButton(this);
	resetFont->setIcon(Icon::get("edit-undo"));
	resetFont->setToolTip(QStringLiteral("Reset font to system default"));
	fontLayout->addWidget(resetFont);

	QToolButton::connect(resetFont, &QAbstractButton::clicked,
		this, &SettingsPage::Interface::onResetFont);

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

	closeToTray = new QCheckBox(QStringLiteral("Close to system tray instead of quitting"), this);
	closeToTray->setToolTip(QStringLiteral("Keep the app running in the tray after closing"));
	closeToTray->setChecked(settings.general.close_to_tray);
	trayOptions->addWidget(closeToTray);

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

	auto &qtSettings = settings.qt();

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

	if (expandAlbumCover != nullptr)
	{
		const auto albumSize = expandAlbumCover->isChecked()
			? lib::album_size::expanded
			: lib::album_size::fixed;

		if (mainWindow != nullptr && qtSettings.album_size != albumSize)
		{
			mainWindow->toggleExpandableAlbum(albumSize);
		}

		qtSettings.album_size = albumSize;
	}

	if (tabbedLibrary != nullptr)
	{
		const auto libraryLayout = tabbedLibrary->isChecked()
			? lib::library_layout::tabbed
			: lib::library_layout::stacked;

		qtSettings.library_layout = libraryLayout;
	}

	if (albumShape != nullptr)
	{
		const auto currentAlbumShape = albumShape->currentAlbumShape();

		if (mainWindow != nullptr && qtSettings.album_shape != currentAlbumShape)
		{
			auto *contextView = mainWindow->findChild<Context::View *>();
			if (contextView != nullptr)
			{
				contextView->reset();
			}
		}

		qtSettings.album_shape = currentAlbumShape;
	}

	if (nativeWindow != nullptr)
	{
		settings.general.native_window = nativeWindow->isChecked();
	}
}

void SettingsPage::Interface::saveAppearance()
{
	auto &qtSettings = settings.qt();

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

	if (toolbarPosition != nullptr)
	{
		const auto newPosition = toolbarPosition->currentIndex() == 1
			? lib::position::bottom
			: lib::position::top;

		if (qtSettings.toolbar_position != newPosition)
		{
			auto *mainWindow = MainWindow::find(parent());
			auto *toolbar = mainWindow->findChild<MainToolBar *>(QString(), Qt::FindDirectChildrenOnly);
			const auto newToolBarArea = MainToolBar::toToolBarArea(newPosition);
			mainWindow->addToolBar(newToolBarArea, toolbar);
		}

		qtSettings.toolbar_position = newPosition;
	}

	if (iconFallback != nullptr)
	{
		settings.general.fallback_icons = iconFallback->isChecked();
	}

	if (fontName.isEmpty())
	{
		QApplication::setFont(getDefaultFont());
	}
	else
	{
		const auto appFont = QApplication::font();
		if (appFont.family() != fontName || appFont.pointSize() != fontSize)
		{
			QApplication::setFont(fontSize > 0
				? QFont(fontName, fontSize)
				: QFont(fontName));
		}
	}

	qtSettings.custom_font_name = fontName.toStdString();
	qtSettings.custom_font_size = fontSize;
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

	if (closeToTray != nullptr)
	{
		if (settings.general.close_to_tray != closeToTray->isChecked())
		{
			reloadTray = true;
		}

		settings.general.close_to_tray = closeToTray->isChecked();
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

void SettingsPage::Interface::onSelectFont(bool /*checked*/)
{
	bool fontSelected;
	const auto font = QFontDialog::getFont(&fontSelected, this);
	if (!fontSelected)
	{
		return;
	}

	currentFont->setText(getFontName(font));
	fontName = font.family();
	fontSize = font.pointSize();
}

void SettingsPage::Interface::onResetFont(bool /*checked*/)
{
	currentFont->setText(getDefaultFontName());
	fontName = QString();
	fontSize = 0;
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

	// First is probably default
	const auto &styles = QStyleFactory::keys();
	if (!styles.isEmpty())
	{
		return styles.first();
	}

	// Assume Fusion
	return QStringLiteral("Fusion");
}

auto SettingsPage::Interface::albumShapes() -> QList<lib::album_shape>
{
	return {
		lib::album_shape::app,
		lib::album_shape::circle,
		lib::album_shape::disc,
		lib::album_shape::none,
	};
}

auto SettingsPage::Interface::getFontName(const QString &family, int pointSize) -> QString
{
	return QStringLiteral("%1 %2pt")
		.arg(family)
		.arg(pointSize);
}

auto SettingsPage::Interface::getFontName(const QFont &font) -> QString
{
	return getFontName(font.family(), font.pointSize());
}

auto SettingsPage::Interface::getDefaultFontName() -> QString
{
	return QStringLiteral("<i>%1</i>")
		.arg(getFontName(getDefaultFont()));
}

auto SettingsPage::Interface::getDefaultFont() -> QFont
{
	return QFontDatabase::systemFont(QFontDatabase::GeneralFont);
}
