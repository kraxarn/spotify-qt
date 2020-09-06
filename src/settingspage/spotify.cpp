#include "../dialog/settingsdialog.hpp"

QWidget *SettingsDialog::spotifySettings()
{
	// Main container for everything
	auto layout = new QVBoxLayout();
	layout->setAlignment(Qt::AlignTop);

	// Executable settings
	auto sptPathLayout = new QHBoxLayout();
	sptPath = new QLineEdit(settings.spotify.path, this);
	sptPath->setPlaceholderText("spotifyd path");
	sptPathLayout->addWidget(sptPath, 1);
	auto sptPathBrowse = new QPushButton("...", this);
	sptPathBrowse->setMaximumWidth(40);
	sptPathBrowse->setFlat(true);
	QAbstractButton::connect(sptPathBrowse, &QAbstractButton::clicked, [this](bool checked) {
		auto filePath = QFileDialog::getOpenFileName(this, "spotifyd path", "/");
		if (!filePath.isEmpty() && sptPath != nullptr)
			sptPath->setText(filePath);
	});
	sptPathLayout->addWidget(sptPathBrowse);
	auto sptOpenLog = new QPushButton(this);
	sptOpenLog->setIcon(Icon::get("folder-txt"));
	sptOpenLog->setToolTip("Open log");
	sptOpenLog->setFlat(true);
	QAbstractButton::connect(sptOpenLog, &QAbstractButton::clicked, [this](bool checked)
	{
		(new ClientHandlerLogDialog(this->parentWidget()))->show();
	});
	sptPathLayout->addWidget(sptOpenLog);
	layout->addLayout(sptPathLayout);

	// Spotifyd version
	sptVersion = new QLabel("(no spotifyd provided)", this);
	if (!settings.spotify.path.isEmpty())
	{
		auto client = spt::ClientHandler::version(settings.spotify.path);
		if (sptVersion != nullptr)
			sptVersion->setText(client);
	}
	sptVersion->setEnabled(false);
	layout->addWidget(sptVersion);

	// Global config
	sptGlobal = new QCheckBox("Use global config", this);
	sptGlobal->setToolTip("Use spotifyd.conf file in either ~/.config/spotifyd or /etc/spotifyd only");
	sptGlobal->setChecked(settings.spotify.globalConfig);
	QCheckBox::connect(sptGlobal, &QCheckBox::stateChanged, this, &SettingsDialog::globalConfigToggle);
	layout->addWidget(sptGlobal);

	// Box and layout for all app specific settings
	sptGroup = new QGroupBox("App specific settings", this);
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
	layout->addWidget(sptGroup);

	// Start with app
	sptAppStart = new QCheckBox("Start with app", this);
	sptAppStart->setToolTip("Start, and close, spotifyd together with the app (only closes with app config)");
	sptAppStart->setChecked(settings.spotify.startClient);
	layout->addWidget(sptAppStart);

	// Always start
	sptAlways = new QCheckBox("Always start", this);
	sptAlways->setToolTip("Always start client, even if there are other devices already available");
	sptAlways->setChecked(settings.spotify.alwaysStart);
	layout->addWidget(sptAlways);

	// Final layout
	auto widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}