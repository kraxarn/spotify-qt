#include "applicationpage.hpp"

ApplicationPage::ApplicationPage(Settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	addTab(app(), "General");
	addTab(appLogs(), "Logs");
}

QWidget *ApplicationPage::app()
{
	auto layout = tabContent();
	auto comboBoxLayout = new QGridLayout();

	// Refresh interval
	auto appRefreshLabel = new QLabel("Refresh interval", this);
	appRefreshLabel->setToolTip("How often to refresh playback status from the Spotify servers");
	comboBoxLayout->addWidget(appRefreshLabel, 0, 0);

	appRefresh = new QComboBox(this);
	appRefresh->addItems({
		"1", "3", "10"
	});
	appRefresh->setEditable(true);
	appRefresh->setCurrentIndex(-1);
	appRefresh->setEditText(QString::number(settings.general.refreshInterval));
	appRefresh->setValidator(new QIntValidator(1, 60, this));
	comboBoxLayout->addWidget(appRefresh, 0, 1);
	comboBoxLayout->addWidget(new QLabel("seconds"), 0, 2);

	// Max queue
	auto maxQueueLabel = new QLabel("Queue limit", this);
	maxQueueLabel->setToolTip("Maximum amount of items allowed to be queued at once");
	comboBoxLayout->addWidget(maxQueueLabel, 1, 0);

	appMaxQueue = new QComboBox(this);
	appMaxQueue->setEditable(true);
	appMaxQueue->setValidator(new QIntValidator(1, 1000, this));
	appMaxQueue->addItems({
		"100", "250", "500"
	});
	appMaxQueue->setCurrentText(QString::number(settings.spotify.maxQueue));
	comboBoxLayout->addWidget(appMaxQueue, 1, 1);
	comboBoxLayout->addWidget(new QLabel("tracks", this), 1, 2);

	layout->addLayout(comboBoxLayout);

	// PulseAudio volume control
	if (isPulse())
	{
		appPulse = new QCheckBox("PulseAudio volume control", this);
		appPulse->setToolTip(
			"Use PulseAudio for volume control instead, only works if listening on same device");
		appPulse->setChecked(settings.general.pulseVolume);
		layout->addWidget(appPulse);
	}

	// MPRIS D-Bus
#ifdef USE_DBUS
	appMedia = new QCheckBox("Media controller", this);
	appMedia->setChecked(settings.general.mediaController);
	appMedia->setToolTip("Enable media controller through the MPRIS D-Bus interface");
	layout->addWidget(appMedia);
#endif

	// Spotify playback order
	appSptOrder = new QCheckBox("Play in order of track number", this);
	appSptOrder->setToolTip("Play tracks in order of track number, instead of by order in list.");
	appSptOrder->setChecked(settings.general.spotifyPlaybackOrder);
	layout->addWidget(appSptOrder);

	// What's new dialog
	appWhatsNew = new QCheckBox("Show what's new on start", this);
	appWhatsNew->setToolTip("Show what's new in the latest version after the app has been updated");
	appWhatsNew->setChecked(settings.general.showChangelog);
	layout->addWidget(appWhatsNew);

	// Other application stuff
	settings.general.showChangelog = appWhatsNew->isChecked();
	settings.general.spotifyPlaybackOrder = appSptOrder->isChecked();

	return Utils::layoutToWidget(layout);
}

QWidget *ApplicationPage::appLogs()
{
	return new LogViewer(this);
}

QIcon ApplicationPage::icon()
{
	return Icon::get("window");
}

QString ApplicationPage::title()
{
	return "Application";
}

bool ApplicationPage::save()
{
	// Media controller
	if (appMedia != nullptr)
	{
		if (appMedia->isChecked() != settings.general.mediaController)
			QMessageBox::information(this, "Media Controller",
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

	// Max queue
	auto maxQueue = appMaxQueue->currentText().toInt(&ok);
	if (!ok || maxQueue <= 0 || maxQueue > 1000)
	{
		applyFail("queue limit");
		return false;
	}
	settings.spotify.maxQueue = maxQueue;

	return true;
}

bool ApplicationPage::isPulse()
{
	// Assume /usr/bin/pactl
	return QFileInfo("/usr/bin/pactl").isExecutable();
}
