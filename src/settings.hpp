#pragma once

#include <QSettings>
#include <QHeaderView>
#include <QCoreApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QStandardPaths>
#include <QDebug>

class Settings
{
public:
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
		QString accessToken, refreshToken, clientId, clientSecret;
	} Account;

	typedef struct {
		QString style, lastPlaylist, lastVersion;
		bool pulseVolume, mediaController, spotifyPlaybackOrder, trayIcon, trayNotifications, trayLightIcon,
			showChangelog, fallbackIcons, fixedWidthTime;
		Palette stylePalette;
		QVector<int> hiddenSongHeaders;
		int songHeaderResizeMode, songHeaderSortBy, refreshInterval;
	} General;

	typedef struct {
		QString path, username;
		bool startClient, globalConfig;
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

	static bool hasMediaControllerSupport();

	static Settings *get();

private:
	Settings();

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

	void load();

	static Settings *set;

	QSettings *settings;
};