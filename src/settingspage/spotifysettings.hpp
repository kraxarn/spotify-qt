#pragma once

#include "settingspage.hpp"

class SpotifySettings: QWidget, public SettingsPage
{
Q_OBJECT

public:
	SpotifySettings(Settings &settings, QWidget *parent);

	QString title() override;
	QWidget *toWidget() override;
	bool applySettings(QWidget *mainWindow) override;

private:
	static bool sptConfigExists();
	void globalConfigToggle(int state);

	QCheckBox *sptAlways = nullptr;
	QCheckBox *sptAppStart = nullptr;
	QCheckBox *sptGlobal = nullptr;
	QComboBox *sptBitrate = nullptr;
	QGroupBox *sptGroup = nullptr;
	QLabel *sptVersion = nullptr;
	QLineEdit *sptPath = nullptr;
	QLineEdit *sptUsername = nullptr;
};


