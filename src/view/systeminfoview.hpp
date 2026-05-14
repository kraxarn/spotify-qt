#pragma once

#include "lib/spotifyappstatus.hpp"
#include "lib/enum/spotifyappversion.hpp"
#include "util/systeminfo.hpp"

#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

class SystemInfoView : public QWidget
{
	Q_OBJECT

public:
	SystemInfoView(const lib::settings &settings, QWidget *parent);

protected:
	void showEvent(QShowEvent *event) override;

private:
	SpotifyAppStatus *appStatus;
	QTextEdit *textInfo;

	SpotifyAppVersion appVersion;

	[[nodiscard]]
	auto systemInfo() const -> lib::qt::system_info;

	void copyToClipboard(bool checked);

	void onAppStatusVersionFinished(SpotifyAppVersion version);
};
