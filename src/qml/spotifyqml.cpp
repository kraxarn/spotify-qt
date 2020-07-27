#include "spotifyqml.hpp"

SpotifyQml::SpotifyQml(QObject *parent)
	: QObject(parent)
{
	spotify = new spt::Spotify(settings);
}