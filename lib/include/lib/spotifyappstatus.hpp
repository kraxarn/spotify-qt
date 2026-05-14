#pragma once

#include "lib/settings.hpp"
#include "lib/enum/spotifyappversion.hpp"
#include "lib/spotify/api.hpp"

#include <QNetworkAccessManager>
#include <QObject>

class SpotifyAppStatus final : public QObject
{
	Q_OBJECT

public:
	SpotifyAppStatus(const lib::settings &settings, QObject *parent);

	void version();

signals:
	void versionFinished(SpotifyAppVersion version);

private:
	QNetworkAccessManager *http;
	const lib::settings &settings;
};
