#include "view/log/spotify.hpp"
#include "spotifyclient/runner.hpp"

Log::Spotify::Spotify(QWidget *parent)
	: Base(parent)
{
}

auto Log::Spotify::getMessages() -> const std::vector<lib::log_message> &
{
	return SpotifyClient::Runner::getLog();
}
