#include "settingspage/spotify.hpp"
#include "mainwindow.hpp"
#include "util/widget.hpp"
#include "util/process.hpp"

#ifdef USE_KEYCHAIN
#include "util/keychain.hpp"
#endif

SettingsPage::Spotify::Spotify(lib::settings &settings, QWidget *parent)
	: SettingsPage::Base(settings, parent)
{
	addTab(spotify(), "General");
	addTab(config(), "Configuration");
}

void SettingsPage::Spotify::showEvent(QShowEvent *event)
{
	QTabWidget::showEvent(event);

	if (sptBackend != nullptr && sptBackend->count() <= 1)
	{
		sptBackend->addItems(backends());
		if (!settings.spotify.backend.empty())
		{
			sptBackend->setCurrentText(QString::fromStdString(settings.spotify.backend));
		}
	}

	if (sptDeviceType != nullptr && sptDeviceType->count() <= 1)
	{
		for (const auto deviceType: deviceTypes())
		{
			if (!addDeviceType(deviceType))
			{
				continue;
			}

			if (settings.spotify.device_type == deviceType)
			{
				sptDeviceType->setCurrentIndex(sptDeviceType->count() - 1);
			}
		}
	}
}

auto SettingsPage::Spotify::spotify() -> QWidget *
{
	auto *content = new QVBoxLayout();
	content->setAlignment(Qt::AlignTop);

	// Executable settings
	auto *pathBox = new QGroupBox(this);
	pathBox->setTitle(QStringLiteral("Path to librespot or spotifyd"));

	auto *pathLayout = new QGridLayout();
	pathBox->setLayout(pathLayout);

	sptPath = new QLineEdit(QString::fromStdString(settings.spotify.path), this);
	pathLayout->addWidget(sptPath, 0, 0);

	auto *sptPathBrowse = new QPushButton(QStringLiteral("..."), this);
	sptPathBrowse->setMaximumWidth(40);
	sptPathBrowse->setFlat(true);
	QAbstractButton::connect(sptPathBrowse, &QAbstractButton::clicked,
		[this](bool /*checked*/)
		{
			auto filePath = QFileDialog::getOpenFileName(this, "Client path", "/");
			if (!filePath.isEmpty() && sptPath != nullptr)
			{
				sptPath->setText(filePath);
			}
		});
	pathLayout->addWidget(sptPathBrowse, 0, 1);

	// Client version
	sptVersion = new QLabel(QStringLiteral("(no client provided)"), this);
	if (!settings.spotify.path.empty())
	{
		const auto path = QString::fromStdString(settings.spotify.path);
		const auto client = SpotifyClient::Helper::version(path);
		sptVersion->setText(client);
	}
	sptVersion->setEnabled(false);
	pathLayout->addWidget(sptVersion, 1, 0);
	content->addWidget(pathBox);

	// Start with app
	sptAppStart = new QCheckBox("Start with app", this);
	sptAppStart->setToolTip("Start, and close, spotify client together with the app "
							"(only closes when using app config)");
	sptAppStart->setChecked(settings.spotify.start_client);
	QCheckBox::connect(sptAppStart, &QCheckBox::stateChanged,
		this, &SettingsPage::Spotify::startClientToggle);
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
		this, &SettingsPage::Spotify::restartClient);

	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		runner = mainWindow->getSpotifyRunner();
		if (runner != nullptr)
		{
			SpotifyClient::Runner::connect(runner, &SpotifyClient::Runner::statusChanged,
				this, &SettingsPage::Spotify::onSpotifyStatusChanged);
		}
	}

	clientStatus = new QLabel(this);
	clientStatus->setEnabled(false);
	statusLayout->addWidget(clientStatus, 1);
	content->addLayout(statusLayout);
	updateClientStatus();

	return Widget::layoutToWidget(content, this);
}

auto SettingsPage::Spotify::isClientRunning() const -> bool
{
	const auto *clientHandler = getClientRunner();
	return clientHandler != nullptr
		&& clientHandler->isRunning();
}

void SettingsPage::Spotify::updateClientStatus()
{
	auto running = isClientRunning();
	setClientStatus(true,
		running
			? QStringLiteral("Stop client")
			: QStringLiteral("Start client"),
		running
			? QStringLiteral("Running")
			: QStringLiteral("Not running"));
}

void SettingsPage::Spotify::restartClient(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow == nullptr)
	{
		return;
	}

	if (isClientRunning())
	{
		mainWindow->stopClient();
		updateClientStatus();
		return;
	}

	setClientStatus(false, QStringLiteral("Starting"),
		QStringLiteral("Please wait..."));

	runner = mainWindow->startClient();
	SpotifyClient::Runner::connect(runner, &SpotifyClient::Runner::statusChanged,
		this, &SettingsPage::Spotify::onSpotifyStatusChanged);
}

auto SettingsPage::Spotify::config() -> QWidget *
{
	auto *content = new QVBoxLayout();
	content->setAlignment(Qt::AlignTop);

	// Global config
	sptGlobal = new QCheckBox("Use global config", this);
	sptGlobal->setToolTip("Use spotifyd.conf file in ~/.config/spotifyd, /etc or "
						  "/etc/xdg/spotifyd (spotifyd only)");
	sptGlobal->setChecked(settings.spotify.global_config);
	QCheckBox::connect(sptGlobal, &QCheckBox::stateChanged,
		this, &SettingsPage::Spotify::globalConfigToggle);
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
		"Normal (96 kbit/s)", "High (160 kbit/s)", "Very high (320 kbit/s)"
	});
	auto bitrate = settings.spotify.bitrate;
	sptBitrate->setCurrentIndex(bitrate == lib::audio_quality::normal
		? 0 : bitrate == lib::audio_quality::high
			? 1 : 2);
	sptLayout->addWidget(sptBitrate, 1, 1);

	// Backend
	auto *backendLabel = new QLabel(QStringLiteral("Audio backend"), sptGroup);
	sptLayout->addWidget(backendLabel, 2, 0);

	sptBackend = new QComboBox(sptGroup);
	sptBackend->addItem(QStringLiteral("Default"));
	sptLayout->addWidget(sptBackend, 2, 1);

	// Device type
	auto *deviceTypeLabel = new QLabel(QStringLiteral("Device type"), sptGroup);
	sptLayout->addWidget(deviceTypeLabel, 3, 0);

	sptDeviceType = new QComboBox(sptGroup);
	sptDeviceType->addItem(QStringLiteral("Default"));
	sptLayout->addWidget(sptDeviceType, 3, 1);

	// Clear password
#ifdef USE_KEYCHAIN
	auto *clearPasswordText = new QLabel(QStringLiteral("Clear saved password"));
	sptLayout->addWidget(clearPasswordText, 4, 0);

	auto *clearPassword = new QPushButton(QStringLiteral("Clear"), this);
	sptLayout->addWidget(clearPassword, 4, 1, Qt::AlignTrailing);

	QAbstractButton::connect(clearPassword, &QAbstractButton::clicked,
		this, &SettingsPage::Spotify::onClearPassword);
#endif

	// librespot discovery
	sptDiscovery = new QCheckBox("Enable discovery");
	sptDiscovery->setToolTip("Enable discovery mode (librespot only)");
	sptDiscovery->setChecked(!settings.spotify.disable_discovery);
	sptLayout->addWidget(sptDiscovery, 5, 0);

	return Widget::layoutToWidget(content, this);
}

auto SettingsPage::Spotify::icon() -> QIcon
{
	return Icon::get("headphones");
}

auto SettingsPage::Spotify::title() -> QString
{
	return "Spotify";
}

auto SettingsPage::Spotify::save() -> bool
{
	auto success = true;

	// Check spotify client path
	if (sptPath != nullptr && !sptPath->text().isEmpty())
	{
		auto client = SpotifyClient::Helper::version(sptPath->text());
		if (client.isEmpty())
		{
			const auto result = Process::exec(sptPath->text(), {});
			const auto title = QStringLiteral("Spotify client path");

			if (result.success())
			{
				warning(title, QStringLiteral("Spotify client not supported"));
			}
			else
			{
				warning(title, result.message().empty()
					? QStringLiteral("Invalid Spotify client")
					: QString("Invalid Spotify client:\n%1")
						.arg(QString::fromStdString(result.message())));

				success = false;
			}
		}

		if (sptVersion != nullptr)
		{
			sptVersion->setText(client);
		}
		settings.spotify.path = sptPath->text().toStdString();
	}

	// librespot has no global config support
	if (sptGlobal != nullptr
		&& sptVersion != nullptr
		&& sptGlobal->isChecked()
		&& sptVersion->text() == "librespot")
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

	// Device type
	if (sptDeviceType != nullptr)
	{
		const auto &selected = sptDeviceType->currentData();

		settings.spotify.device_type = selected.canConvert<short>()
			? static_cast<lib::device_type>(selected.value<short>())
			: lib::device_type::unknown;
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
		settings.spotify.bitrate = bitrate == 0
			? lib::audio_quality::normal : bitrate == 1
				? lib::audio_quality::high
				: lib::audio_quality::very_high;
	}

	if (sptAlways != nullptr)
	{
		settings.spotify.always_start = sptAlways->isChecked();
	}

	if (sptDiscovery != nullptr)
	{
		settings.spotify.disable_discovery = !sptDiscovery->isChecked();
	}

	return success;
}

void SettingsPage::Spotify::globalConfigToggle(int state)
{
	if (sptGroup == nullptr)
	{
		return;
	}

	sptGroup->setEnabled(state == Qt::Unchecked);
}

void SettingsPage::Spotify::startClientToggle(int state)
{
	if (sptAlways == nullptr)
	{
		return;
	}

	sptAlways->setEnabled(state == Qt::Checked);
}

auto SettingsPage::Spotify::sptConfigExists() -> bool
{
	const auto homeLocation = QStandardPaths::standardLocations(QStandardPaths::HomeLocation)[0];

	// Config is either ~/.config/spotifyd/spotifyd.conf or /etc/spotifyd/spotifyd.conf or /etc/xdg/spotifyd/spotifyd.conf or ~\AppData\Roaming\spotifyd\spotifyd.conf
	return QFile::exists(QString("%1/.config/spotifyd/spotifyd.conf").arg(homeLocation))
		|| QFile::exists(QStringLiteral("/etc/spotifyd/spotifyd.conf"))
		|| QFile::exists(QStringLiteral("/etc/xdg/spotifyd/spotifyd.conf"))
		|| QFile::exists(QString(R"(%1\AppData\Roaming\spotifyd\spotifyd.conf)").arg(homeLocation));
}

auto SettingsPage::Spotify::getPath() const -> QString
{
	return QString::fromStdString(settings.spotify.path);
}

auto SettingsPage::Spotify::backends() -> QStringList
{
	return SpotifyClient::Helper::availableBackends(getPath());
}

auto SettingsPage::Spotify::deviceTypes() -> QList<lib::device_type>
{
	QList<lib::device_type> deviceTypes{
		lib::device_type::computer,
		lib::device_type::speaker,
	};

	// Always show current, if changed manually
	const auto current = settings.spotify.device_type;
	if (current != lib::device_type::unknown && !deviceTypes.contains(current))
	{
		deviceTypes.append(current);
	}

	return deviceTypes;
}

auto SettingsPage::Spotify::addDeviceType(lib::device_type deviceType) -> bool
{
	const auto deviceTypeString = lib::enums<lib::device_type>::to_string(deviceType);
	if (deviceTypeString.empty())
	{
		return false;
	}

	const auto text = QString::fromStdString(lib::strings::capitalize(deviceTypeString));
	const auto data = static_cast<short>(deviceType);

	sptDeviceType->addItem(text, data);
	return true;
}

auto SettingsPage::Spotify::getClientRunner() const -> const SpotifyClient::Runner *
{
	auto *mainWindow = MainWindow::find(parentWidget());
	return mainWindow == nullptr
		? nullptr
		: mainWindow->getSpotifyRunner();
}

void SettingsPage::Spotify::setClientStatus(bool enabled,
	const QString &start, const QString &status)
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

void SettingsPage::Spotify::onSpotifyStatusChanged(const QString &status)
{
	setClientStatus(true,
		isClientRunning()
			? QStringLiteral("Stop client")
			: QStringLiteral("Start client"),
		status.isEmpty()
			? QStringLiteral("Running")
			: status);
}

#ifdef USE_KEYCHAIN
void SettingsPage::Spotify::onClearPassword(bool /*checked*/)
{
	const auto username = QString::fromStdString(settings.spotify.username);
	if (username.isEmpty())
	{
		QMessageBox::information(this, QStringLiteral("No username"),
			QStringLiteral("No username saved to clear password for."));
		return;
	}

	const auto password = Keychain::getPassword(username);
	if (password.isEmpty())
	{
		QMessageBox::information(this, QStringLiteral("No password"),
			QString("No password saved for user \"%1\".").arg(username));
		return;
	}

	if (!Keychain::clearPassword(username))
	{
		QMessageBox::warning(this, QStringLiteral("Failed"),
			QStringLiteral("Failed to clear password, check the log for details."));
		return;
	}

	QMessageBox::information(this, QStringLiteral("Cleared"),
		QString("Password cleared for user \"%1\".").arg(username));
}
#endif
