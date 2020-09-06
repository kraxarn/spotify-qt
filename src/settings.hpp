#pragma once

#include "enum/palette.hpp"
#include "enum/playlistorder.hpp"
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
	virtual ~Settings();

	QJsonDocument legacyToJson();
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
	} General;

	typedef struct {
		QString path, username;
		bool startClient, globalConfig, alwaysStart;
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
	QString sptPath();
	bool sptStartClient();
	QString sptUser();
	int sptBitrate();
	bool sptGlobalConfig();
	bool fixedWidthTime();

	QSettings *settings;
	QMutex mutex;
};