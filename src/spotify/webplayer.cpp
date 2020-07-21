#include "webplayer.hpp"

using namespace spt;

WebPlayer::WebPlayer(const QString &token) : token(token)
{
}

QString WebPlayer::html()
{
	return QString(
		"<script src=\"https://sdk.scdn.co/spotify-player.js\"></script>"
		"<script>"
		"window.onSpotifyWebPlaybackSDKReady = () => {"
		"const token = \"%1\";"
		"const player = new Spotify.Player({"
		"name: \"spotify-qt\","
		"getOAuthToken: cb => { cb(token) }"
		"});"
		"player.connect();"
		"};"
		"</script>").arg(token);
}