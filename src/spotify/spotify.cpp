#include "spotify/spotify.hpp"

spt::Spotify::Spotify(lib::settings &settings, const lib::http_client &httpClient,
	lib::spt::request &request, QObject *parent)
	: QObject(parent),
	lib::spt::api(settings, httpClient, request)
{
}
