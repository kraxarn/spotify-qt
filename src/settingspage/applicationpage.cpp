#include "applicationpage.hpp"

ApplicationPage::ApplicationPage(lib::settings &settings, QWidget *parent)
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
	appRefresh->setEditText(QString::number(settings.general.refresh_interval));
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
	appMaxQueue->setCurrentText(QString::number(settings.spotify.max_queue));
	comboBoxLayout->addWidget(appMaxQueue, 1, 1);
	comboBoxLayout->addWidget(new QLabel("tracks", this), 1, 2);

	layout->addLayout(comboBoxLayout);

	// PulseAudio volume control
	if (isPulse())
	{
		appPulse = new QCheckBox("PulseAudio volume control", this);
		appPulse->setToolTip(
			"Use PulseAudio for volume control instead, only works if listening on same device");
		appPulse->setChecked(settings.general.pulse_volume);
		layout->addWidget(appPulse);
	}

	// MPRIS D-Bus
#ifdef USE_DBUS
	appMedia = new QCheckBox("Media controller", this);
	appMedia->setChecked(settings.general.media_controller);
	appMedia->setToolTip("Enable media controller through the MPRIS D-Bus interface");
	layout->addWidget(appMedia);
#endif

	// Spotify playback order
	appSptOrder = new QCheckBox("Play in order of track number", this);
	appSptOrder->setToolTip("Play tracks in order of track number, instead of by order in list.");
	appSptOrder->setChecked(settings.general.spotify_playback_order);
	layout->addWidget(appSptOrder);

	// What's new dialog
	appWhatsNew = new QCheckBox("Show what's new on start", this);
	appWhatsNew->setToolTip("Show what's new in the latest version after the app has been updated");
	appWhatsNew->setChecked(settings.general.show_changelog);
	layout->addWidget(appWhatsNew);

	return Utils::layoutToWidget(layout);
}

QWidget *ApplicationPage::appLogs()
{
	return new LogView(this);
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
		if (appMedia->isChecked() != settings.general.media_controller)
		{
			QMessageBox::information(this, "Media Controller",
				"Please restart the application to apply changes");
		}
		settings.general.media_controller = appMedia->isChecked();
	}

	// PulseAudio volume
	if (appPulse != nullptr)
		settings.general.pulse_volume = appPulse->isChecked();

	// Refresh interval
	auto ok = false;
	auto interval = appRefresh->currentText().toInt(&ok);
	if (!ok || interval <= 0 || interval > 60)
	{
		applyFail("refresh interval");
		return false;
	}
	settings.general.refresh_interval = interval;

	// Max queue
	auto maxQueue = appMaxQueue->currentText().toInt(&ok);
	if (!ok || maxQueue <= 0 || maxQueue > 1000)
	{
		applyFail("queue limit");
		return false;
	}
	settings.spotify.max_queue = maxQueue;

	// Other application stuff
	settings.general.show_changelog = appWhatsNew->isChecked();
	settings.general.spotify_playback_order = appSptOrder->isChecked();

	return true;
}

bool ApplicationPage::isPulse()
{
	// Assume /usr/bin/pactl
	return QFileInfo("/usr/bin/pactl").isExecutable();
}
