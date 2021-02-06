#pragma once

#include "settingspage.hpp"
#include "../util/icon.hpp"

#include <QComboBox>
#include <QGroupBox>

class SpotifyPage: public SettingsPage
{
public:
	SpotifyPage(lib::settings &settings, QWidget *parent);

	QIcon icon() override;
	QString title() override;
	bool save() override;

private:
	QCheckBox *sptAlways = nullptr;
	QCheckBox *sptAppStart = nullptr;
	QCheckBox *sptGlobal = nullptr;
	QCheckBox *sptKeyring = nullptr;
	QComboBox *sptBackend = nullptr;
	QComboBox *sptBitrate = nullptr;
	QGroupBox *sptGroup = nullptr;
	QLabel *sptVersion = nullptr;
	QLineEdit *sptPath = nullptr;
	QLineEdit *sptUsername = nullptr;
	QCheckBox *sptDiscovery = nullptr;

	void globalConfigToggle(int state);
	void startClientToggle(int state);
	static bool sptConfigExists();
	QStringList backends();

	QWidget *spotify();
	QWidget *config();
	QWidget *logs();
};
