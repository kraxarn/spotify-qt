#include "spotifyqml.hpp"

SpotifyQml::SpotifyQml(QObject *parent)
	: QObject(parent)
{
	spotify = new spt::Spotify(settings);

	spt::Spotify::connect(spotify, &spt::Spotify::gotPlayback, [this](const spt::Playback &playback)
	{
		refreshed(playback);
	});

	auto timer = new QTimer(this);
	QTimer::connect(timer, &QTimer::timeout, this, &SpotifyQml::refresh);
	refresh();
	timer->start(1000);
}

void SpotifyQml::refresh()
{
	// TODO: For now this is a copy from MainWindow.cpp

	if (refreshCount < 0
		|| ++refreshCount >= settings.general.refreshInterval
		|| current.progressMs + 1000 > current.item.duration)
	{
		spotify->requestCurrentPlayback();
		refreshCount = 0;
		return;
	}
	// Assume last refresh was 1 sec ago
	if (!current.isPlaying)
		return;
	current.progressMs += 1000;
	refreshed(current);
}

void SpotifyQml::refreshed(const spt::Playback &playback)
{
	// Update the actual UI and stuff from QML
	current = playback;
	emit playbackChanged();
}

QJsonArray SpotifyQml::getPlaylists()
{
	QJsonArray items;
	for (auto &playlist : spotify->playlists())
		items.append(playlist.toJson());
	return items;
}

SpotifyQml::~SpotifyQml()
{
	delete spotify;
}

QJsonObject SpotifyQml::getPlayback()
{
	return current.toJson();
}
