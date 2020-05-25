#pragma once

#include "lib/json.hpp"

#include <iostream>
#include <fstream>
#include <string>

#include <QSettings>
#include <QHeaderView>
#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QStandardPaths>

class Settings
{
public:
	Settings();
	virtual ~Settings();

	QJsonDocument legacyToJson();
	void save() const;

	enum Palette
	{
		paletteApp   = 0,	// Default app palette
		paletteStyle = 1,	// Palette from current style
		paletteDark  = 2,	// Custom dark palette
	};

	typedef struct {
		std::string accessToken, refreshToken, clientId, clientSecret;
	} Account;

	typedef struct {
		std::string style, lastPlaylist, lastVersion;
		bool pulseVolume, mediaController, spotifyPlaybackOrder, trayIcon, trayNotifications, trayLightIcon,
			showChangelog, fallbackIcons, fixedWidthTime;
		Palette stylePalette;
		std::vector<int> hiddenSongHeaders;
		int songHeaderResizeMode, songHeaderSortBy, refreshInterval;
	} General;

	typedef struct {
		std::string path, username;
		bool startClient, globalConfig;
		int bitrate;
	} Spotify;

	Account account;
	General general;
	Spotify spotify;

	void removeClient();
	void removeTokens();

	bool darkTheme();
	void setDarkTheme(bool value);

	static std::string fileName();

private:

	QString accessToken();

	QString refreshToken();

	QString clientId();

	QString clientSecret();

	QString style();

	bool pulseVolume();

	QString lastPlaylist();

	Palette stylePalette();

	bool mediaController();

	bool sptPlaybackOrder();

	QList<int> hiddenSongHeaders();

	bool trayIcon();

	bool trayNotifications();

	bool trayLightIcon();

	QHeaderView::ResizeMode songHeaderResizeMode();

	int songHeaderSortBy();

	int refreshInterval();

	QString lastVersion();

	bool showChangelog();

	bool useFallbackIcons();

	// Spotify settings

	QString sptPath();

	bool sptStartClient();

	QString sptUser();

	int sptBitrate();

	bool sptGlobalConfig();

	bool fixedWidthTime();

	void load();

	QSettings *settings;
};