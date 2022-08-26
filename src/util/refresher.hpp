#pragma once

#include "util/icon.hpp"
#include "util/image.hpp"
#include "spotify/spotify.hpp"

#include <QSplashScreen>
#include <QPixmap>
#include <QPainter>

class Refresher: public QObject
{
Q_OBJECT

public:
	Refresher(lib::settings &settings, spt::Spotify &spotify);

	void refresh(const std::function<void(bool)> &callback);

private:
	lib::settings &settings;
	spt::Spotify &spotify;
};
