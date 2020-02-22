#include "settingsdialog.hpp"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
	// Main layout
	auto mainLayout = new QGridLayout();
	// Add app settings to main layout
	mainLayout->addWidget(appSettings(), 0, 0, Qt::AlignTop);
	// Add spotifyd settings to main layout
	mainLayout->addWidget(spotifydSettings(), 0, 1);
	// Buttons
	auto okButton = new QPushButton("OK");
	QPushButton::connect(okButton, &QPushButton::clicked, [=](bool checked) {
		applySettings();
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
	// Embedded player
	auto appPlayer = new QCheckBox("Embedded player", this);
	appPlayer->setToolTip("Embedded player, removes the need for an external player, but not as reliable");
	appLayout->addWidget(appPlayer, 0, 0, 1, 2);
	// Theme
	auto appThemeLabel = new QLabel("Theme", this);
	appThemeLabel->setToolTip("Theme used throughout the application");
	appLayout->addWidget(appThemeLabel, 1, 0);
	appTheme = new QComboBox(this);
	appTheme->addItems(QStyleFactory::keys());
	for (auto &style : QStyleFactory::keys())
		if (style.toLower() == QApplication::style()->objectName())
			appTheme->setCurrentText(style);
	appLayout->addWidget(appTheme, 1, 1);
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
	return appSettings;
}

QGroupBox *SettingsDialog::spotifydSettings()
{
	// Main container for everything
	auto sptSettings = new QGroupBox("spotifyd", this);
	auto sptMainLayout = new QVBoxLayout(this);
	sptSettings->setLayout(sptMainLayout);
	// Executable settings
	auto sptPath = new QLineEdit("/usr/bin/spotifyd", this);
	sptPath->setPlaceholderText("spotifyd path");
	sptMainLayout->addWidget(sptPath);
	// Spotifyd version
	QProcess process(this);
	process.start("/usr/bin/spotifyd", {
		"--version"
	});
	process.waitForFinished();
	auto sptVersion = new QLabel(process.readAllStandardOutput(), this);
	sptMainLayout->addWidget(sptVersion);
	// Override spotifyd config
	auto sptOverride = new QCheckBox("Override spotifyd config", this);
	sptOverride->setToolTip("Ignore spotifyd config and use specified config instead");
	sptMainLayout->addWidget(sptOverride);
	// Layout for all settings
	auto sptLayout = new QGridLayout();
	sptLayout->setEnabled(false);
	sptMainLayout->addLayout(sptLayout);
	// Username
	sptLayout->addWidget(new QLabel("Username", this), 0, 0);
	auto username = new QLineEdit(this);
	sptLayout->addWidget(username, 0, 1);
	// Password method
	sptLayout->addWidget(new QLabel("Password method", this));
	auto pwMethod = new QComboBox(this);
	pwMethod->addItems({
		"Plain text", "Command", "Keyring"
	});
	pwMethod->setCurrentIndex(-1);
	sptLayout->addWidget(pwMethod);
	// Password
	auto pwTitle = new QLabel("Password", this);
	sptLayout->addWidget(pwTitle);
	auto pw = new QLineEdit(this);
	sptLayout->addWidget(pw);
	// Change password entry depending on method
	// ...
	// Backend
	sptLayout->addWidget(new QLabel("Backend", this));
	auto sptBackend = new QComboBox(this);
	sptBackend->addItems(backends());
	sptLayout->addWidget(sptBackend);
	// Device name
	sptLayout->addWidget(new QLabel("Device name", this));
	auto sptName = new QLineEdit(this);
	sptLayout->addWidget(sptName);
	// Bitrate
	sptLayout->addWidget(new QLabel("Bitrate", this));
	auto sptBitrate = new QComboBox(this);
	sptBitrate->addItems({
		"Low (96 kbit/s)", "Medium (160 kbit/s)", "High (320 kbit/s)"
	});
	sptLayout->addWidget(sptBitrate);
	// Volume normalization
	auto sptVolNorm = new QCheckBox("Volume normalization", this);
	sptLayout->addWidget(sptVolNorm, 6, 0, 1, 2);
	// Final layout
	return sptSettings;
}

QStringList SettingsDialog::backends()
{
	QProcess process(this);
	process.start("/usr/bin/spotifyd", {
		"--help"
	});
	process.waitForFinished();
	QString out(process.readAllStandardOutput());
	QStringList lines(out.split('\n'));
	for (auto &line : lines)
		if (line.trimmed().startsWith("-b"))
			return line.right(line.length() - line.indexOf(':') - 2).remove(']').split(", ");
	return QStringList();
}

void SettingsDialog::applySettings()
{
	// Set theme
	QApplication::setStyle(appTheme->currentText());
}

