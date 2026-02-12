#include "view/log/spotify.hpp"
#include "spotifyclient/runner.hpp"

Log::Spotify::Spotify(QWidget *parent)
	: Base(parent)
{
}

auto Log::Spotify::getMessages() -> const QList<LogMessage> &
{
	return SpotifyClient::Runner::getLog();
}
