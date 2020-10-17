#pragma once

#include "enum/palette.hpp"
#include "enum/playlistorder.hpp"
#include "enum/spotifycontext.hpp"
#include "util/utils.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QSettings>
#include <QStandardPaths>

class Settings
{
public:
	Settings();

	QJsonObject toJson();
	void fromJson(const QJsonObject &json);
	void save();
	void load();

	typedef struct {
		QString accessToken, refreshToken, clientId, clientSecret;
	} Account;

	typedef struct {
		QString style, lastPlaylist, lastVersion;
		bool pulseVolume, mediaController, spotifyPlaybackOrder, trayIcon, trayNotifications, trayLightIcon,
			showChangelog, fallbackIcons, fixedWidthTime, trayAlbumArt, showContextInfo, singleClickPlay;
		Palette stylePalette;
		QVector<int> hiddenSongHeaders;
		int songHeaderResizeMode, songHeaderSortBy, refreshInterval, lastVolume;
		PlaylistOrder playlistOrder;
		QStringList customPlaylistOrder;
		SpotifyContext trackNumbers;
	} General;

	typedef struct {
		QString path, username;
		bool startClient, globalConfig, alwaysStart, keyringPassword;
		int bitrate;
	} Spotify;

	Account account;
	General general;
	Spotify spotify;

	void removeClient();
	void removeTokens();

	bool darkTheme() const;
	void setDarkTheme(bool value);

	static QString fileName();
	static QString filePath();

	static bool hasMediaControllerSupport();

private:
	QMutex mutex;
};