#include "spotifysettings.hpp"

SpotifySettings::SpotifySettings(Settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	// Executable settings
	auto sptPathLayout = new QHBoxLayout();
	sptPath = new QLineEdit(settings.spotify.path, parent);
	sptPath->setPlaceholderText("spotifyd path");
	sptPathLayout->addWidget(sptPath, 1);
	auto sptPathBrowse = new QPushButton("...", parent);
	sptPathBrowse->setMaximumWidth(40);
	sptPathBrowse->setFlat(true);
	QAbstractButton::connect(sptPathBrowse, &QAbstractButton::clicked, [this](bool checked)
	{
		auto filePath = QFileDialog::getOpenFileName(this->parent, "spotifyd path", "/");
		if (!filePath.isEmpty() && sptPath != nullptr)
			sptPath->setText(filePath);
	});
	sptPathLayout->addWidget(sptPathBrowse);
	auto sptOpenLog = new QPushButton(parent);
	sptOpenLog->setIcon(Icon::get("folder-txt"));
	sptOpenLog->setToolTip("Open log");
	sptOpenLog->setFlat(true);
	QAbstractButton::connect(sptOpenLog, &QAbstractButton::clicked, [this](bool checked)
	{
		(new ClientHandlerLogDialog(this->parent))->show();
	});
	sptPathLayout->addWidget(sptOpenLog);
	page->addLayout(sptPathLayout);

	// Spotifyd version
	sptVersion = new QLabel("(no spotifyd provided)", parent);
	if (!settings.spotify.path.isEmpty())
	{
		auto client = spt::ClientHandler::version(settings.spotify.path);
		if (sptVersion != nullptr)
			sptVersion->setText(client);
	}
	sptVersion->setEnabled(false);
	page->addWidget(sptVersion);

	// Global config
	sptGlobal = new QCheckBox("Use global config", parent);
	sptGlobal->setToolTip("Use spotifyd.conf file in either ~/.config/spotifyd or /etc/spotifyd only");
	sptGlobal->setChecked(settings.spotify.globalConfig);
	QCheckBox::connect(sptGlobal, &QCheckBox::stateChanged, this, &SpotifySettings::globalConfigToggle);
	page->addWidget(sptGlobal);

	// Box and layout for all app specific settings
	sptGroup = new QGroupBox("App specific settings", parent);
	sptGroup->setEnabled(!sptGlobal->isChecked());
	auto sptLayout = new QGridLayout();

	// Username
	sptLayout->addWidget(new QLabel("Username", sptGroup), 0, 0);
	sptUsername = new QLineEdit(settings.spotify.username, sptGroup);
	sptLayout->addWidget(sptUsername, 0, 1);

	// Bitrate
	sptLayout->addWidget(new QLabel("Quality", sptGroup), 1, 0);
	sptBitrate = new QComboBox(sptGroup);
	sptBitrate->addItems(
		{
			"Low (96 kbit/s)", "Medium (160 kbit/s)", "High (320 kbit/s)"
		}
	);
	auto bitrate = settings.spotify.bitrate;
	sptBitrate->setCurrentIndex(bitrate == 96 ? 0 : bitrate == 160 ? 1 : 2);
	sptLayout->addWidget(sptBitrate, 1, 1);
	sptGroup->setLayout(sptLayout);
	page->addWidget(sptGroup);

	// Start with app
	sptAppStart = new QCheckBox("Start with app", parent);
	sptAppStart->setToolTip("Start, and close, spotifyd together with the app (only closes with app config)");
	sptAppStart->setChecked(settings.spotify.startClient);
	page->addWidget(sptAppStart);

	// Always start
	sptAlways = new QCheckBox("Always start", parent);
	sptAlways->setToolTip("Always start client, even if there are other devices already available");
	sptAlways->setChecked(settings.spotify.alwaysStart);
	page->addWidget(sptAlways);
}

QString SpotifySettings::title()
{
	return "Spotify";
}

bool SpotifySettings::applySettings(QWidget *mainWindow)
{
	// Check spotify client path
	if (!sptPath->text().isEmpty())
	{
		auto client = spt::ClientHandler::version(sptPath->text());
		if (client.isEmpty())
		{
			applyFail("spotifyd path");
			return false;
		}
		sptVersion->setText(client);
		settings.spotify.path = sptPath->text();
	}

	// Spotify global config
	if (sptGlobal->isChecked() && !sptConfigExists())
		QMessageBox::warning(
			parent,
			"spotifyd config not found",
			QString("Couldn't find a config file for spotifyd. You may experience issues."));
	settings.spotify.globalConfig = sptGlobal->isChecked();

	// Other Spotify stuff
	settings.spotify.startClient = sptAppStart->isChecked();
	settings.spotify.username = sptUsername->text();
	auto bitrate = sptBitrate->currentIndex();
	settings.spotify.bitrate = bitrate == 0 ? 96 : bitrate == 1 ? 160 : 320;
	settings.spotify.alwaysStart = sptAlways->isChecked();
}

void SpotifySettings::globalConfigToggle(int state)
{
	sptGroup->setEnabled(state == Qt::Unchecked);
}

bool SpotifySettings::sptConfigExists()
{
	// Config is either ~/.config/spotifyd/spotifyd.conf or /etc/spotifyd/spotifyd.conf
	return QFile(
		QString("%1/.config/spotifyd/spotifyd.conf")
			.arg(QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0])).exists()
		|| QFile("/etc/spotifyd/spotifyd.conf").exists();
}