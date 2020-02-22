#include "settingsdialog.hpp"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
	// Main layout
	auto mainLayout = new QGridLayout();
	// Add app settings to main layout
	mainLayout->addWidget(appSettings(), 0, 0, Qt::AlignTop);
	// Add spotifyd settings to main layout
	mainLayout->addWidget(spotifySettings(), 0, 1);
	// Buttons
	auto okButton = new QPushButton("OK");
	QPushButton::connect(okButton, &QPushButton::clicked, [=](bool checked) {
		if (applySettings())
			accept();
	});
	auto applyButton = new QPushButton("Apply");
	QPushButton::connect(applyButton, &QPushButton::clicked, [=](bool checked) {
		applySettings();
	});
	auto cancelButton = new QPushButton("Cancel");
	QPushButton::connect(cancelButton, &QPushButton::clicked, [=](bool checked) {
		accept();
	});
	auto buttons = new QHBoxLayout();
	buttons->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
	buttons->addWidget(okButton);
	buttons->addWidget(applyButton);
	buttons->addWidget(cancelButton);
	mainLayout->addLayout(buttons, 1, 0, 1, 2);
	// Set layout
	setWindowTitle("Settings");
	setLayout(mainLayout);
}

QGroupBox *SettingsDialog::appSettings()
{
	auto appSettings = new QGroupBox("Application", this);
	auto appLayout = new QGridLayout(this);
	appSettings->setLayout(appLayout);
	// Theme
	auto appThemeLabel = new QLabel("Theme", this);
	appThemeLabel->setToolTip("Theme used throughout the application");
	appLayout->addWidget(appThemeLabel, 0, 0);
	appTheme = new QComboBox(this);
	appTheme->addItems(QStyleFactory::keys());
	for (auto &style : QStyleFactory::keys())
		if (style.toLower() == QApplication::style()->objectName())
			appTheme->setCurrentText(style);
	appLayout->addWidget(appTheme, 0, 1);
	// Refresh interval
	auto appRefreshLabel = new QLabel("Refresh interval", this);
	appRefreshLabel->setToolTip("How often to refresh playback status from the Spotify servers");
	appLayout->addWidget(appRefreshLabel);
	auto appRefresh = new QComboBox(this);
	appRefresh->addItems({
		"Fast (3s)", "Medium (5s)", "Slow (10s)"
	});
	appRefresh->setCurrentIndex(1);
	appLayout->addWidget(appRefresh);
	// Embedded player
	auto appPlayer = new QCheckBox("Embedded player", this);
	appPlayer->setToolTip("Embedded player, removes the need for an external player, but not as reliable");
	appLayout->addWidget(appPlayer, 2, 0, 1, 2);
	// Start client
	sptStartClient = new QCheckBox("Autostart Spotify client", this);
	sptStartClient->setToolTip("Start specified Spotify client on start together with the app");
	sptStartClient->setChecked(settings.sptStartClient());
	appLayout->addWidget(sptStartClient, 3, 0, 1, 2);
	return appSettings;
}

QGroupBox *SettingsDialog::spotifySettings()
{
	// Main container for everything
	auto sptSettings = new QGroupBox("Spotify", this);
	auto sptMainLayout = new QVBoxLayout(this);
	sptSettings->setLayout(sptMainLayout);
	// Executable settings
	sptPath = new QLineEdit(settings.sptPath(), this);
	sptPath->setPlaceholderText("librespot/spotifyd path");
	sptMainLayout->addWidget(sptPath);
	// Spotifyd version
	sptVersion = new QLabel("(no client provided)", this);
	if (!settings.sptPath().isEmpty())
		sptVersion->setText(sptClient(settings.sptPath()));
	sptVersion->setEnabled(false);
	sptMainLayout->addWidget(sptVersion);
	// Override spotifyd config
	auto sptOverride = new QCheckBox("Override spotifyd config", this);
	sptOverride->setToolTip("Ignore spotifyd config and use specified config instead");
	sptOverride->setChecked(true);
	sptMainLayout->addWidget(sptOverride);
	// Layout for all settings
	auto sptLayout = new QGridLayout();
	sptLayout->setEnabled(false);
	sptMainLayout->addLayout(sptLayout);
	// Username
	sptLayout->addWidget(new QLabel("Username", this), 0, 0);
	sptUsername = new QLineEdit(settings.sptUser(), this);
	sptLayout->addWidget(sptUsername, 0, 1);
	// Device name
	sptLayout->addWidget(new QLabel("Device name", this));
	auto sptName = new QLineEdit("spotify-qt", this);
	sptLayout->addWidget(sptName);
	// Bitrate
	sptLayout->addWidget(new QLabel("Bitrate", this));
	auto sptBitrate = new QComboBox(this);
	sptBitrate->addItems({
		"Low (96 kbit/s)", "Medium (160 kbit/s)", "High (320 kbit/s)"
	});
	sptBitrate->setCurrentIndex(2);
	sptLayout->addWidget(sptBitrate);
	// Volume normalization
	auto sptVolNorm = new QCheckBox("Volume normalization", this);
	sptLayout->addWidget(sptVolNorm, 6, 0, 1, 2);
	// Final layout
	return sptSettings;
}

QString SettingsDialog::sptClient(const QString &path)
{
	// Check if it exists
	QFileInfo file(path);
	if (!file.exists())
		return QString();
	// Check if either client
	if (file.baseName() != "librespot" && file.baseName() != "spotifyd")
		return QString();
	// Prepare process
	QProcess process;
	// Get version info
	if (file.baseName() == "librespot")
	{
		// --name is always required
		// --backend ? is just used to not actually start
		process.start(file.absoluteFilePath(), {
			"--backend", "?", "--name", ""
		});
		process.waitForFinished();
		// Version info is output to stderr for some reason
		QString out(process.readAllStandardError());
		auto right = out.right(out.length() - out.indexOf("] librespot") - 2);
		return right.left(right.indexOf("."));
	}
	// spotifyd has a simple --version
	process.start(file.absoluteFilePath(), {
		"--version"
	});
	process.waitForFinished();
	// Entire stdout is version
	return process.readAllStandardOutput();
}

bool SettingsDialog::applySettings()
{
	// Set theme
	QApplication::setStyle(appTheme->currentText());
	settings.setStyle(appTheme->currentText());

	// Check spotify client path
	if (!sptPath->text().isEmpty())
	{
		auto client = sptClient(sptPath->text());
		if (client.isEmpty())
		{
			applyFail("spotify client");
			return false;
		}
		sptVersion->setEnabled(true);
		sptVersion->setText(client);
		settings.setSptPath(sptPath->text());
	}

	// Other Spotify stuff
	settings.setSptStartClient(sptStartClient->isChecked());
	settings.setSptUser(sptUsername->text());

	// Everything is fine
	return true;
}

void SettingsDialog::applyFail(const QString &setting)
{
	QMessageBox::warning(this,
		"Failed to apply settings",
		QString("Failed to apply setting \"%1\". Check your settings and try again.").arg(setting));
}