#include "../dialog/settingsdialog.hpp"

QWidget *SettingsDialog::interfaceSettings()
{
	auto layout = new QVBoxLayout();
	layout->setAlignment(Qt::AlignTop);

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

	// Final layout
	auto widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}

bool SettingsDialog::hasIconTheme()
{
	return !QIcon::fromTheme("media-playback-start").isNull();
}