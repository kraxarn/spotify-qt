#pragma once

#include "settingspage.hpp"
#include "../util/icon.hpp"
#include "client/clienthelper.hpp"
#include "view/clienthandlerlogview.hpp"
#include "client/clienthandler.hpp"

#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

class SpotifyPage: public SettingsPage
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

	static constexpr int low = 96;
	static constexpr int medium = 160;
	static constexpr int high = 320;

	void globalConfigToggle(int state);
	void startClientToggle(int state);
	static auto sptConfigExists() -> bool;
	auto backends() -> QStringList;

	void updateClientStatus();
	void restartClient(bool checked);
	auto isClientRunning() const -> bool;

	auto getClientHandler() const -> const spt::ClientHandler *;

	auto spotify() -> QWidget *;
	auto config() -> QWidget *;
	auto logs() -> QWidget *;
};
