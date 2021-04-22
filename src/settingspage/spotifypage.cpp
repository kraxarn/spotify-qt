#include "spotifypage.hpp"

#include "mainwindow.hpp"

SpotifyPage::SpotifyPage(lib::settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	addTab(spotify(), "General");
	addTab(config(), "Configuration");
	addTab(logs(), "Logs");
}

auto SpotifyPage::spotify() -> QWidget *
{
	auto *content = new QVBoxLayout();
	content->setAlignment(Qt::AlignTop);

	// Executable settings
	auto *sptPathLayout = new QHBoxLayout();
	sptPath = new QLineEdit(QString::fromStdString(settings.spotify.path), this);
	sptPath->setPlaceholderText("Client path");
	sptPathLayout->addWidget(sptPath, 1);
	auto *sptPathBrowse = new QPushButton("...", this);
	sptPathBrowse->setMaximumWidth(40);
	sptPathBrowse->setFlat(true);
	QAbstractButton::connect(sptPathBrowse, &QAbstractButton::clicked, [this]
		(bool /*checked*/)
	{
		auto filePath = QFileDialog::getOpenFileName(this, "Client path", "/");
		if (!filePath.isEmpty() && sptPath != nullptr)
		{
			sptPath->setText(filePath);
		}
	});
	sptPathLayout->addWidget(sptPathBrowse);
	content->addLayout(sptPathLayout);

	// Client version
	sptVersion = new QLabel("(no client provided)", this);
	if (!settings.spotify.path.empty())
	{
		auto client = spt::ClientHelper::version(QString::fromStdString(settings.spotify.path));
		sptVersion->setText(client);
	}
	sptVersion->setEnabled(false);
	content->addWidget(sptVersion);

	// Start with app
	sptAppStart = new QCheckBox("Start with app", this);
	sptAppStart->setToolTip("Start, and close, spotify client together with the app "
							"(only closes when using app config)");
	sptAppStart->setChecked(settings.spotify.start_client);
	QCheckBox::connect(sptAppStart, &QCheckBox::stateChanged,
		this, &SpotifyPage::startClientToggle);
	content->addWidget(sptAppStart);

	// Always start
	sptAlways = new QCheckBox("Always start", this);
	sptAlways->setToolTip("Always start client, even if there are other devices already available");
	sptAlways->setChecked(settings.spotify.always_start);
	sptAlways->setEnabled(sptAppStart->isChecked());
	content->addWidget(sptAlways);

	// Client restart
	auto *statusLayout = new QHBoxLayout();
	startClient = new QPushButton(this);
	startClient->setEnabled(MainWindow::find(parentWidget()) != nullptr);
	statusLayout->addWidget(startClient);

	QAbstractButton::connect(startClient, &QAbstractButton::clicked,
		this, &SpotifyPage::restartClient);

	clientStatus = new QLabel(this);
	clientStatus->setEnabled(false);
	statusLayout->addWidget(clientStatus, 1);
	content->addLayout(statusLayout);
	updateClientStatus();

	return Utils::layoutToWidget(content);
}

auto SpotifyPage::isClientRunning() const -> bool
{
	const auto *clientHandler = getClientHandler();
	return clientHandler != nullptr
		&& clientHandler->isRunning();
}

void SpotifyPage::updateClientStatus()
{
	auto running = isClientRunning();
	setClientStatus(true,
		running ? "Stop client" : "Start client",
		running ? "Running" : "Not running");
}

void SpotifyPage::restartClient(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		if (isClientRunning())
		{
			mainWindow->stopClient();
		}
		else
		{
			setClientStatus(false, "Starting", "Please wait...");

			if (!mainWindow->startClient())
			{
				setClientStatus(true, "Start client", "Failed to start");
			}
			else
			{
				const auto *clientHandler = getClientHandler();
				if (clientHandler != nullptr)
				{
					auto success = clientHandler->waitForStarted();
					setClientStatus(true,
						success ? "Stop client" : "Start client",
						success ? "Running" : "Failed to start");
					return;
				}
				updateClientStatus();
			}

			return;
		}
	}
	updateClientStatus();
}

auto SpotifyPage::config() -> QWidget *
{
	auto *content = new QVBoxLayout();
	content->setAlignment(Qt::AlignTop);

	// Global config
	sptGlobal = new QCheckBox("Use global config", this);
	sptGlobal->setToolTip("Use spotifyd.conf file in ~/.config/spotifyd, /etc or "
						  "/etc/xdg/spotifyd (spotifyd only)");
	sptGlobal->setChecked(settings.spotify.global_config);
	QCheckBox::connect(sptGlobal, &QCheckBox::stateChanged, this, &SpotifyPage::globalConfigToggle);
	content->addWidget(sptGlobal);

	// Box and layout for all app specific settings
	sptGroup = new QGroupBox("App specific settings", this);
	sptGroup->setEnabled(!sptGlobal->isChecked());
	auto *sptLayout = new QGridLayout();
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
	sptBitrate->setCurrentIndex(bitrate == low ? 0 : bitrate == medium ? 1 : 2);
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
		sptKeyring->setChecked(settings.spotify.keyring_password);
		sptLayout->addWidget(sptKeyring, 3, 0);
	}
#endif

	// librespot discovery
	sptDiscovery = new QCheckBox("Enable discovery");
	sptDiscovery->setToolTip("Enable discovery mode (librespot only)");
	sptDiscovery->setChecked(!settings.spotify.disable_discovery);
	sptLayout->addWidget(sptDiscovery, 4, 0);

	return Utils::layoutToWidget(content);
}

auto SpotifyPage::logs() -> QWidget *
{
	return new ClientHandlerLogView(MainWindow::find(parentWidget()));
}

auto SpotifyPage::icon() -> QIcon
{
	return Icon::get("headphones");
}

auto SpotifyPage::title() -> QString
{
	return "Spotify";
}

auto SpotifyPage::save() -> bool
{
	// Check spotify client path
	if (sptPath != nullptr && !sptPath->text().isEmpty())
	{
		auto client = spt::ClientHelper::version(sptPath->text());
		if (client.isEmpty())
		{
			applyFail("spotifyd path");
			return false;
		}

		if (sptVersion != nullptr)
		{
			sptVersion->setText(client);
		}
		settings.spotify.path = sptPath->text().toStdString();
	}

	// librespot has no global config support
	if (sptGlobal != nullptr && sptVersion != nullptr
		&& sptGlobal->isChecked() && sptVersion->text() == "librespot")
	{
		warning("librespot",
			"Global config is not available when using librespot");
		sptGlobal->setChecked(false);
	}

	// Spotify global config
	if (sptGlobal != nullptr)
	{
		if (sptGlobal->isChecked() && !sptConfigExists())
		{
			warning("spotifyd config not found",
				QString("Couldn't find a config file for spotifyd. You may experience issues."));
		}
		settings.spotify.global_config = sptGlobal->isChecked();
	}

	// Backend
	if (sptBackend != nullptr)
	{
		settings.spotify.backend = sptBackend->currentIndex() == 0
			? std::string()
			: sptBackend->currentText().toStdString();
	}

	// Other Spotify stuff
	if (sptAppStart != nullptr)
	{
		settings.spotify.start_client = sptAppStart->isChecked();
	}
	if (sptUsername != nullptr)
	{
		settings.spotify.username = sptUsername->text().toStdString();
	}
	if (sptBitrate != nullptr)
	{
		auto bitrate = sptBitrate->currentIndex();
		settings.spotify.bitrate = bitrate == 0 ? low : bitrate == 1 ? medium : high;
	}
	if (sptAlways != nullptr)
	{
		settings.spotify.always_start = sptAlways->isChecked();
	}
	if (sptKeyring != nullptr)
	{
		settings.spotify.keyring_password = sptKeyring->isChecked();
	}
	if (sptDiscovery != nullptr)
	{
		settings.spotify.disable_discovery = !sptDiscovery->isChecked();
	}

	return true;
}

void SpotifyPage::globalConfigToggle(int state)
{
	if (sptGroup == nullptr)
	{
		return;
	}

	sptGroup->setEnabled(state == Qt::Unchecked);
}

void SpotifyPage::startClientToggle(int state)
{
	if (sptAlways == nullptr)
	{
		return;
	}

	sptAlways->setEnabled(state == Qt::Checked);
}

auto SpotifyPage::sptConfigExists() -> bool
{
	// Config is either ~/.config/spotifyd/spotifyd.conf or /etc/spotifyd/spotifyd.conf
	return QFile(QString("%1/.config/spotifyd/spotifyd.conf")
		.arg(QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0])).exists()
		|| QFile("/etc/spotifyd/spotifyd.conf").exists();
}

auto SpotifyPage::backends() -> QStringList
{
	return spt::ClientHandler(settings, this).availableBackends();
}

auto SpotifyPage::getClientHandler() const -> const spt::ClientHandler *
{
	auto *mainWindow = MainWindow::find(parentWidget());
	return mainWindow == nullptr
		? nullptr
		: mainWindow->getClientHandler();
}

void SpotifyPage::setClientStatus(bool enabled, const QString &start, const QString &status)
{
	if (startClient != nullptr)
	{
		startClient->setText(start);
		startClient->setEnabled(enabled);
	}
	if (clientStatus != nullptr)
	{
		clientStatus->setText(status);
	}
}
