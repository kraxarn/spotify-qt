#pragma once

#include "lib/json.hpp"

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
	explicit Settings();
	virtual ~Settings();

	QJsonDocument legacyToJson();
	void save();

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

	QString fileName();

private:

	QString accessToken();
	void setAccessToken(QString &value);

	QString refreshToken();
	void setRefreshToken(QString &value);

	QString clientId();
	void setClientId(const QString &value);

	QString clientSecret();
	void setClientSecret(const QString &value);

	QString style();
	void setStyle(const QString &value);

	bool pulseVolume();
	void setPulseVolume(bool value);

	QString lastPlaylist();
	void setLastPlaylist(const QString &value);

	Palette stylePalette();
	void setStylePalette(Palette value);

	bool mediaController();
	void setMediaController(bool value);

	bool sptPlaybackOrder();
	void setSptPlaybackOrder(bool value);

	QList<int> hiddenSongHeaders();
	void setHiddenSongHeaders(const QList<int> &values);
	void addHiddenSongHeader(int id);
	void removeHiddenSongHeader(int id);

	bool trayIcon();
	void setTrayIcon(bool value);

	bool trayNotifications();
	void setTrayNotifications(bool value);

	bool trayLightIcon();
	void setTrayLightIcon(bool value);

	QHeaderView::ResizeMode songHeaderResizeMode();
	void setSongHeaderResizeMode(QHeaderView::ResizeMode value);

	int songHeaderSortBy();
	void setSongHeaderSortBy(int value);

	int refreshInterval();
	void setRefreshInterval(int value);

	QString lastVersion();
	void setLastVersion(const QString &value);

	bool showChangelog();
	void setShowChangelog(bool value);

	bool useFallbackIcons();
	void setUseFallbackIcons(bool value);

	// Spotify settings

	QString sptPath();
	void setSptPath(const QString &value);

	bool sptStartClient();
	void setSptStartClient(bool value);

	QString sptUser();
	void setSptUser(const QString &value);

	int sptBitrate();
	void setSptBitrate(int value);

	bool sptGlobalConfig();
	void setSptGlobalConfig(bool value);

	bool fixedWidthTime();
	void setFixedWidthTime(bool value);

	void load();

	QSettings *settings;
};