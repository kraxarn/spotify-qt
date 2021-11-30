#pragma once

#include "settingspage/base.hpp"
#include "lib/enum/audioquality.hpp"
#include "util/icon.hpp"
#include "client/clienthelper.hpp"
#include "view/clienthandlerlogview.hpp"
#include "client/clienthandler.hpp"

#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

class SpotifyPage: public SettingsPage::Base
{
public:
	SpotifyPage(lib::settings &settings, QWidget *parent);

	auto icon() -> QIcon override;
	auto title() -> QString override;
	auto save() -> bool override;

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

	QPushButton *startClient = nullptr;
	QLabel *clientStatus = nullptr;

	lib::paths *paths = nullptr;

	void globalConfigToggle(int state);
	void startClientToggle(int state);
	static auto sptConfigExists() -> bool;
	auto backends() -> QStringList;

	void updateClientStatus();
	void restartClient(bool checked);
	auto isClientRunning() const -> bool;

	auto getClientHandler() const -> const spt::ClientHandler *;
	void setClientStatus(bool enabled, const QString &start, const QString &status);

	auto spotify() -> QWidget *;
	auto config() -> QWidget *;
	auto logs() -> QWidget *;
};
