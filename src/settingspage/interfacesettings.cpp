#include "interfacesettings.hpp"

InterfaceSettings::InterfaceSettings(Settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	// Dark theme
	itfDark = new QCheckBox("Dark theme", parent);
	itfDark->setToolTip("Use custom dark theme");
	itfDark->setChecked(settings.darkTheme());
	page->addWidget(itfDark);

	// Song header resize mode
	itfResizeAuto = new QCheckBox("Auto resize track list headers", parent);
	itfResizeAuto->setToolTip("Automatically resize track list headers to fit content");
	itfResizeAuto->setChecked(settings.general.songHeaderResizeMode == QHeaderView::ResizeToContents);
	page->addWidget(itfResizeAuto);

	// Always use fallback icons (if system icons are an option)
	if (hasIconTheme())
	{
		itfIcFallback = new QCheckBox("Always use fallback icons", parent);
		itfIcFallback->setToolTip("Always use bundled fallback icons, even if system icons are available");
		itfIcFallback->setChecked(settings.general.fallbackIcons);
		page->addWidget(itfIcFallback);
	}

	// Monospace remaining time
	itfMonoTime = new QCheckBox("Fixed width remaining time", parent);
	itfMonoTime->setToolTip("Use a fixed width for remaining time to avoid resizing");
	itfMonoTime->setChecked(settings.general.fixedWidthTime);
	page->addWidget(itfMonoTime);

	// Context info
	itfContextInfo = new QCheckBox("Show context information", parent);
	itfContextInfo->setToolTip("Show what context music is currently playing from above current track");
	itfContextInfo->setChecked(settings.general.showContextInfo);
	page->addWidget(itfContextInfo);
}

QString InterfaceSettings::title()
{
	return "Interface";
}

bool InterfaceSettings::applySettings(QWidget* mainWindow)
{
	auto window = dynamic_cast<MainWindow*>(mainWindow);

	// Set theme
	auto changeTheme = itfDark->isChecked() != settings.darkTheme();
	if (changeTheme)
	{
		QMessageBox::information(
			parent, "Dark Theme",
			"Please restart the application to fully apply selected theme");
		settings.setDarkTheme(itfDark->isChecked());
		QApplication::setStyle(settings.general.style);
		Utils::applyPalette(settings.general.stylePalette);
	}

	// Song header resize mode
	auto resizeMode =
		itfResizeAuto->isChecked()
		? QHeaderView::ResizeToContents
		: QHeaderView::Interactive;
	if (resizeMode != settings.general.songHeaderResizeMode && window != nullptr)
		window->getSongsTree()->header()->setSectionResizeMode(resizeMode);
	settings.general.songHeaderResizeMode = resizeMode;

	// Other stuff
	if (itfIcFallback != nullptr)
		settings.general.fallbackIcons = itfIcFallback->isChecked();
	if (window != nullptr)
		window->setFixedWidthTime(itfMonoTime->isChecked());
	settings.general.fixedWidthTime = itfMonoTime->isChecked();
	settings.general.showContextInfo = itfContextInfo->isChecked();
}

bool InterfaceSettings::hasIconTheme()
{
	return !QIcon::fromTheme("media-playback-start").isNull();
}