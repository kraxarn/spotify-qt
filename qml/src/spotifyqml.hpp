#pragma once

#include "common.hpp"

class SpotifyQml: public QObject
{
Q_OBJECT
	Q_PROPERTY(QJsonArray playlists READ getPlaylists NOTIFY playlistsChanged)
	Q_PROPERTY(QJsonObject playback READ getPlayback NOTIFY playbackChanged)
	QML_ELEMENT

public:
	explicit SpotifyQml(QObject *parent = nullptr);
	~SpotifyQml() override;

	QJsonArray getPlaylists();
	QJsonObject getPlayback();

signals:
	void playlistsChanged();
	void playbackChanged();

private:
	spt::Spotify *spotify = nullptr;
	Settings settings;
	spt::Playback current;

	int refreshCount = -1;

	void refresh();
	void refreshed(const spt::Playback &playback);
};


