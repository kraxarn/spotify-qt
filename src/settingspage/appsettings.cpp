#include "appsettings.hpp"

AppSettings::AppSettings(Settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	// Refresh interval
	auto appRefreshLayout = new QHBoxLayout();
	auto appRefreshLabel = new QLabel("Refresh interval", parent);
	appRefreshLabel->setToolTip("How often to refresh playback status from the Spotify servers");
	appRefreshLayout->addWidget(appRefreshLabel);
	appRefresh = new QComboBox(parent);
	appRefresh->addItems(
		{
			"1", "3", "10"
		}
	);
	appRefresh->setEditable(true);
	appRefresh->setCurrentIndex(-1);
	appRefresh->setEditText(QString::number(settings.general.refreshInterval));
	appRefresh->setValidator(new QIntValidator(1, 60, parent));
	appRefreshLayout->addWidget(appRefresh);
	appRefreshLayout->addWidget(new QLabel("seconds"));
	page->addLayout(appRefreshLayout);

	// PulseAudio volume control
	if (isPulse())
	{
		appPulse = new QCheckBox("PulseAudio volume control", parent);
		appPulse->setToolTip(
			"Use PulseAudio for volume control instead, only works if listening on same device");
		appPulse->setChecked(settings.general.pulseVolume);
		page->addWidget(appPulse);
	}

	// MPRIS D-Bus
#ifdef Q_OS_LINUX
	appMedia = new QCheckBox("Media controller", parent);
	appMedia->setChecked(settings.general.mediaController);
	appMedia->setToolTip("Enable media controller through the MPRIS D-Bus interface");
	page->addWidget(appMedia);
#endif

	// Spotify playback order
	appSptOrder = new QCheckBox("Spotify playback order", parent);
	appSptOrder->setToolTip("Use Spotify playback order instead of app list order");
	appSptOrder->setChecked(settings.general.spotifyPlaybackOrder);
	page->addWidget(appSptOrder);

	// What's new dialog
	appWhatsNew = new QCheckBox("Show what's new on start", parent);
	appWhatsNew->setToolTip("Show what's new in the latest version after the app has been updated");
	appWhatsNew->setChecked(settings.general.showChangelog);
	page->addWidget(appWhatsNew);

	// Single click to play tracks
	appOneClick = new QCheckBox("Single click to play tracks", parent);
	appOneClick->setToolTip("Play tracks, instead of selecting only, from single click");
	appOneClick->setChecked(settings.general.singleClickPlay);
	page->addWidget(appOneClick);
}

bool AppSettings::isPulse()
{
	// Assume /usr/bin/pactl
	return QFileInfo("/usr/bin/pactl").isExecutable();
}

QString AppSettings::title()
{
	return "Application";
}

bool AppSettings::applySettings(QWidget*)
{
	// Media controller
	if (appMedia != nullptr)
	{
		if (appMedia->isChecked() != settings.general.mediaController)
			QMessageBox::information(
				parent, "Media Controller",
				"Please restart the application to apply changes");
		settings.general.mediaController = appMedia->isChecked();
	}

	// PulseAudio volume
	if (appPulse != nullptr)
		settings.general.pulseVolume = appPulse->isChecked();

	// Refresh interval
	auto ok = false;
	auto interval = appRefresh->currentText().toInt(&ok);
	if (!ok || interval <= 0 || interval > 60)
	{
		applyFail("refresh interval");
		return false;
	}
	settings.general.refreshInterval = interval;

	// Other stuff
	settings.general.showChangelog = appWhatsNew->isChecked();
	settings.general.spotifyPlaybackOrder = appSptOrder->isChecked();
	settings.general.singleClickPlay = appOneClick->isChecked();

	return true;
}
