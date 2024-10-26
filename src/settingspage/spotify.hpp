#pragma once

#include "settingspage/base.hpp"
#include "lib/enum/audioquality.hpp"
#include "util/icon.hpp"
#include "spotifyclient/runner.hpp"

#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>

namespace SettingsPage
{
	class Spotify: public Base
	{
	public:
		Spotify(lib::settings &settings, QWidget *parent);

		auto icon() -> QIcon override;
		auto title() -> QString override;
		auto save() -> bool override;

	protected:
		void showEvent(QShowEvent *event) override;

	private:
		QCheckBox *sptAlways = nullptr;
		QCheckBox *sptAppStart = nullptr;
		QCheckBox *sptGlobal = nullptr;
		QComboBox *sptBackend = nullptr;
		QComboBox *sptDeviceType = nullptr;
		QComboBox *sptBitrate = nullptr;
		QGroupBox *sptGroup = nullptr;
		QLabel *sptVersion = nullptr;
		QLineEdit *sptPath = nullptr;
		QCheckBox *sptDiscovery = nullptr;
		QLineEdit *sptAdditionalArguments = nullptr;

		QPushButton *startClient = nullptr;
		QLabel *clientStatus = nullptr;

		const SpotifyClient::Runner *runner = nullptr;

		void globalConfigToggle(int state);
		void startClientToggle(int state);
		static auto sptConfigExists() -> bool;

		auto getPath() const -> QString;
		auto backends() -> QStringList;

		auto deviceTypes() -> QList<lib::device_type>;
		auto addDeviceType(lib::device_type deviceType) -> bool;

		void updateClientStatus();
		void restartClient(bool checked);
		auto isClientRunning() const -> bool;

		auto getClientRunner() const -> const SpotifyClient::Runner *;
		void setClientStatus(bool enabled, const QString &start, const QString &status);

		auto spotify() -> QWidget *;
		auto config() -> QWidget *;

		void onSpotifyStatusChanged(const QString &status);
	};
}
