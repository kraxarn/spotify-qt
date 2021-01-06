#pragma once

#include "enum/palette.hpp"
#include "enum/playlistorder.hpp"
#include "enum/spotifycontext.hpp"
#include "log.hpp"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QSettings>
#include <QStandardPaths>
#include <QVector>

Q_DECLARE_METATYPE(Palette)
Q_DECLARE_METATYPE(PlaylistOrder)
Q_DECLARE_METATYPE(SpotifyContext)

class Settings
{
public:
	explicit Settings();

	QJsonObject toJson() const;
	void fromJson(const QJsonObject &json);
	void save();
	void load();

	typedef struct Account
	{
		QString accessToken;
		QString refreshToken;
		QString clientId;
		QString clientSecret;
		long lastRefresh = 0;
	} Account;

	typedef struct General
	{
		Palette stylePalette = PaletteApp;
		PlaylistOrder playlistOrder = PlaylistOrderDefault;
		QString lastPlaylist;
		QString lastVersion;
		QString style;
		QStringList customPlaylistOrder;
		QVector<int> hiddenSongHeaders;
		SpotifyContext trackNumbers = ContextAll;
		bool fallbackIcons = false;
		bool fixedWidthTime = true;
		bool mediaController = false;
		bool pulseVolume = false;
		bool relativeAdded = true;
		bool showChangelog = true;
		bool showContextInfo = true;
		bool spotifyPlaybackOrder = true;
		bool trayAlbumArt = false;
		bool trayIcon = true;
		bool trayLightIcon = false;
		bool trayNotifications = false;
		int lastVolume = 0;
		int refreshInterval = 3;
		int songHeaderResizeMode = 3; // QHeaderView::ResizeToContents
		int songHeaderSortBy = -1;
	} General;

	typedef struct Spotify
	{
		QString backend;
		QString path;
		QString username;
		bool alwaysStart = true;
		bool globalConfig = false;
		bool keyringPassword = false;
		bool startClient = false;
		int bitrate = 320;
		int maxQueue = 500;
	} Spotify;

	Account account;
	General general;
	Spotify spotify;

	void removeClient();
	void removeTokens();

	bool darkTheme() const;
	void setDarkTheme(bool value);

	QString fileName() const;
	QString filePath() const;

	std::map<std::string, std::vector<std::string>> validate() const;

private:
	QMutex mutex;

	template<typename T>
	void setValue(const QJsonObject &json, const QString &key, T &value);
};