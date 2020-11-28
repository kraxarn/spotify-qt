#include "spotifypage.hpp"

SpotifyPage::SpotifyPage(lib::Settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	addTab(spotify(), "General");
	addTab(config(), "Configuration");
	addTab(logs(), "Logs");
}

QWidget *SpotifyPage::spotify()
{
	auto content = new QVBoxLayout();
	content->setAlignment(Qt::AlignTop);

	// Executable settings
	auto sptPathLayout = new QHBoxLayout();
	sptPath = new QLineEdit(QString::fromStdString(settings.spotify.path), this);
	sptPath->setPlaceholderText("Client path");
	sptPathLayout->addWidget(sptPath, 1);
	auto sptPathBrowse = new QPushButton("...", this);
	sptPathBrowse->setMaximumWidth(40);
	sptPathBrowse->setFlat(true);
	QAbstractButton::connect(sptPathBrowse, &QAbstractButton::clicked, [this](bool checked)
	{
		auto filePath = QFileDialog::getOpenFileName(this, "Client path", "/");
		if (!filePath.isEmpty() && sptPath != nullptr)
			sptPath->setText(filePath);
	});
	sptPathLayout->addWidget(sptPathBrowse);
	content->addLayout(sptPathLayout);

	// Client version
	sptVersion = new QLabel("(no client provided)", this);
	if (!settings.spotify.path.empty())
	{
		auto client = spt::ClientHandler::version(QString::fromStdString(settings.spotify.path));
		if (sptVersion != nullptr)
			sptVersion->setText(client);
	}
	sptVersion->setEnabled(false);
	content->addWidget(sptVersion);

	// Max queue
	auto maxQueueLayout = new QHBoxLayout();
	auto maxQueueLabel = new QLabel("Queue limit", this);
	maxQueueLabel->setToolTip("Maximum amount of items allowed to be queued at once");
	maxQueueLayout->addWidget(maxQueueLabel);
	sptMaxQueue = new QComboBox(this);
	sptMaxQueue->setEditable(true);
	sptMaxQueue->setValidator(new QIntValidator(1, 1000, this));
	sptMaxQueue->addItems({
		"100", "250", "500"
	});
	sptMaxQueue->setCurrentText(QString::number(settings.spotify.maxQueue));
	maxQueueLayout->addWidget(sptMaxQueue);
	maxQueueLayout->addWidget(new QLabel("tracks", this));
	content->addLayout(maxQueueLayout);

	// Start with app
	sptAppStart = new QCheckBox("Start with app", this);
	sptAppStart->setToolTip("Start, and close, spotify client together with the app (only closes when using app config)");
	sptAppStart->setChecked(settings.spotify.startClient);
	QCheckBox::connect(sptAppStart, &QCheckBox::stateChanged, this, &SpotifyPage::startClientToggle);
	content->addWidget(sptAppStart);

	// Always start
	sptAlways = new QCheckBox("Always start", this);
	sptAlways->setToolTip("Always start client, even if there are other devices already available");
	sptAlways->setChecked(settings.spotify.alwaysStart);
	sptAlways->setEnabled(sptAppStart->isChecked());
	content->addWidget(sptAlways);

	return Utils::layoutToWidget(content);
}

QWidget *SpotifyPage::config()
{
	auto content = new QVBoxLayout();
	content->setAlignment(Qt::AlignTop);

	// Global config
	sptGlobal = new QCheckBox("Use global config", this);
	sptGlobal->setToolTip("Use spotifyd.conf file in either ~/.config/spotifyd or /etc/spotifyd only");
	sptGlobal->setChecked(settings.spotify.globalConfig);
	QCheckBox::connect(sptGlobal, &QCheckBox::stateChanged, this, &SpotifyPage::globalConfigToggle);
	content->addWidget(sptGlobal);

	// Box and layout for all app specific settings
	sptGroup = new QGroupBox("App specific settings", this);
	sptGroup->setEnabled(!sptGlobal->isChecked());
	auto sptLayout = new QGridLayout();
	sptGroup->setLayout(sptLayout);
	content->addWidget(sptGroup);

	// Username
	sptLayout->addWidget(new QLabel("Username", sptGroup), 0, 0);
	sptUsername = new QLineEdit(QString::fromStdString(settings.spotify.username), sptGroup);
	sptLayout->addWidget(sptUsername, 0, 1);

	// Bitrate
	sptLayout->addWidget(new QLabel("Quality", sptGroup), 1, 0);
	sptBitrate = new QComboBox(sptGroup);
	sptBitrate->addItems({
		"Low (96 kbit/s)", "Medium (160 kbit/s)", "High (320 kbit/s)"
	});
	auto bitrate = settings.spotify.bitrate;
	sptBitrate->setCurrentIndex(bitrate == 96 ? 0 : bitrate == 160 ? 1 : 2);
	sptLayout->addWidget(sptBitrate, 1, 1);

	// Backend
	sptLayout->addWidget(new QLabel("Backend", sptGroup), 2, 0);
	sptBackend = new QComboBox(sptGroup);
	sptBackend->addItem("Auto");
	sptBackend->addItems(backends());
	sptBackend->setCurrentText(QString::fromStdString(settings.spotify.backend));
	sptLayout->addWidget(sptBackend, 2, 1);

	// KWallet keyring for password
#ifdef USE_DBUS
	if (KWallet(QString::fromStdString(settings.spotify.username)).isEnabled())
	{
		sptKeyring = new QCheckBox("Save password in keyring", this);
		sptKeyring->setToolTip("Store password in keyring (using KWallet)");
		sptKeyring->setChecked(settings.spotify.keyringPassword);
		sptLayout->addWidget(sptKeyring, 3, 0);
	}
#endif

	return Utils::layoutToWidget(content);
}

QWidget *SpotifyPage::logs()
{
	return new ClientHandlerLogView(findMainWindow());
}

QIcon SpotifyPage::icon()
{
	return Icon::get("headphones");
}

QString SpotifyPage::title()
{
	return "Spotify";
}

bool SpotifyPage::save()
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
		settings.spotify.path = sptPath->text().toStdString();
	}

	// Max queue
	auto ok = false;
	auto maxQueue = sptMaxQueue->currentText().toInt(&ok);
	if (!ok || maxQueue <= 0 || maxQueue > 1000)
	{
		applyFail("queue limit");
		return false;
	}
	settings.spotify.maxQueue = maxQueue;

	// librespot has no global config support
	if (sptGlobal->isChecked() && sptVersion->text() == "librespot")
	{
		warning("librespot",
			"Global config is not available when using librespot");
		sptGlobal->setChecked(false);
	}

	// Spotify global config
	if (sptGlobal->isChecked() && !sptConfigExists())
		warning("spotifyd config not found",
			QString("Couldn't find a config file for spotifyd. You may experience issues."));
	settings.spotify.globalConfig = sptGlobal->isChecked();

	// Backend
	settings.spotify.backend = sptBackend->currentIndex() == 0
		? std::string()
		: sptBackend->currentText().toStdString();

	// Other Spotify stuff
	settings.spotify.startClient = sptAppStart->isChecked();
	settings.spotify.username = sptUsername->text().toStdString();
	auto bitrate = sptBitrate->currentIndex();
	settings.spotify.bitrate = bitrate == 0 ? 96 : bitrate == 1 ? 160 : 320;
	settings.spotify.alwaysStart = sptAlways->isChecked();
	if (sptKeyring != nullptr)
		settings.spotify.keyringPassword = sptKeyring->isChecked();

	return true;
}

void SpotifyPage::globalConfigToggle(int state)
{
	sptGroup->setEnabled(state == Qt::Unchecked);
}

void SpotifyPage::startClientToggle(int state)
{
	sptAlways->setEnabled(state == Qt::Checked);
}

bool SpotifyPage::sptConfigExists()
{
	// Config is either ~/.config/spotifyd/spotifyd.conf or /etc/spotifyd/spotifyd.conf
	return QFile(QString("%1/.config/spotifyd/spotifyd.conf")
		.arg(QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0])).exists()
		|| QFile("/etc/spotifyd/spotifyd.conf").exists();
}

QStringList SpotifyPage::backends()
{
	return spt::ClientHandler(settings, this).availableBackends();
}
