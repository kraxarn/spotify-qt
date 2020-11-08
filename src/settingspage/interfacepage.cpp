#include "interfacepage.hpp"

InterfacePage::InterfacePage(Settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	// Dark theme
	itfDark = new QCheckBox("Dark theme", this);
	itfDark->setToolTip("Use custom dark theme");
	itfDark->setChecked(settings.darkTheme());
	layout->addWidget(itfDark);

	// Song header resize mode
	itfResizeAuto = new QCheckBox("Auto resize track list headers", this);
	itfResizeAuto->setToolTip("Automatically resize track list headers to fit content");
	itfResizeAuto->setChecked(settings.general.songHeaderResizeMode == QHeaderView::ResizeToContents);
	layout->addWidget(itfResizeAuto);

	// Always use fallback icons (if system icons are an option)
	if (hasIconTheme())
	{
		itfIcFallback = new QCheckBox("Always use fallback icons", this);
		itfIcFallback->setToolTip("Always use bundled fallback icons, even if system icons are available");
		itfIcFallback->setChecked(settings.general.fallbackIcons);
		layout->addWidget(itfIcFallback);
	}

	// Monospace remaining time
	itfMonoTime = new QCheckBox("Fixed width remaining time", this);
	itfMonoTime->setToolTip("Use a fixed width for remaining time to avoid resizing");
	itfMonoTime->setChecked(settings.general.fixedWidthTime);
	layout->addWidget(itfMonoTime);

	// Context info
	itfContextInfo = new QCheckBox("Show context information", this);
	itfContextInfo->setToolTip("Show what context music is currently playing from above current track");
	itfContextInfo->setChecked(settings.general.showContextInfo);
	layout->addWidget(itfContextInfo);

	// Track numbers
	itfTrackNum = new QCheckBox("Show track numbers", this);
	itfTrackNum->setToolTip("Show track numbers next to tracks in the list");
	itfTrackNum->setChecked(settings.general.trackNumbers == ContextAll);
	layout->addWidget(itfTrackNum);
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
//	if (resizeMode != settings.general.songHeaderResizeMode && window != nullptr)
//		window->getSongsTree()->header()->setSectionResizeMode(resizeMode); // TODO
	settings.general.songHeaderResizeMode = resizeMode;

	// Track numbers
//	if (window != nullptr)
//		window->toggleTrackNumbers(itfTrackNum->isChecked()); // TODO
	settings.general.trackNumbers = itfTrackNum->isChecked()
		? ContextAll
		: ContextNone;

	// Other interface stuff
	if (itfIcFallback != nullptr)
		settings.general.fallbackIcons = itfIcFallback->isChecked();
//	if (window != nullptr)
//		window->setFixedWidthTime(itfMonoTime->isChecked()); // TODO
	settings.general.fixedWidthTime = itfMonoTime->isChecked();
	settings.general.showContextInfo = itfContextInfo->isChecked();

	return false;
}

bool InterfacePage::hasIconTheme()
{
	return !QIcon::fromTheme("media-playback-start").isNull();
}
