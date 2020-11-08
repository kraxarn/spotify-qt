#pragma once

#include "settingspage.hpp"
#include "../util/icon.hpp"

#include <QComboBox>

class SpotifyPage: public SettingsPage
{
public:
	SpotifyPage(Settings &settings, QWidget *parent);

	QIcon icon() override;
	QString title() override;
	bool save() override;

private:
	QCheckBox *sptAlways = nullptr;
	QCheckBox *sptAppStart = nullptr;
	QCheckBox *sptGlobal = nullptr;
	QCheckBox *sptKeyring = nullptr;
	QComboBox *sptBitrate = nullptr;
	QGroupBox *sptGroup = nullptr;
	QLabel *sptVersion = nullptr;
	QLineEdit *sptPath = nullptr;
	QLineEdit *sptUsername = nullptr;

	void globalConfigToggle(int state);
	void startClientToggle(int state);
	static bool sptConfigExists();
};
