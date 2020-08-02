#pragma once

#include "common.hpp"

class SpotifyQml: public QObject
{
Q_OBJECT
	Q_PROPERTY(QJsonArray playlists READ getPlaylists NOTIFY playlistsChanged)
	Q_PROPERTY(QJsonObject playback READ getPlayback NOTIFY playbackChanged)
	Q_PROPERTY(QJsonObject settings READ getSettings NOTIFY settingsChanged)
	QML_ELEMENT

public:
	explicit SpotifyQml(QObject *parent = nullptr);
	~SpotifyQml() override;

	QJsonArray getPlaylists();
	QJsonObject getPlayback();
	QJsonObject getSettings();

	Q_INVOKABLE QJsonArray getPlaylistTracks(const QString &playlistId);

signals:
	void playlistsChanged();
	void playbackChanged();
	void settingsChanged();

private:
	spt::Spotify *spotify = nullptr;
	Settings settings;
	spt::Playback current;

	int refreshCount = -1;

	void refresh();
	void refreshed(const spt::Playback &playback);
};


