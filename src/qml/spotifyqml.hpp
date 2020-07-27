#pragma once

#include "../spotify/spotify.hpp"
#include "../settings.hpp"

#include <QObject>

class SpotifyQml: public QObject
{
	Q_OBJECT

public:
	explicit SpotifyQml(QObject *parent = nullptr);

private:
	spt::Spotify *spotify = nullptr;
	Settings settings;
};


