#pragma once

#include "enum/palette.hpp"
#include "enum/playlistorder.hpp"
#include "enum/spotifycontext.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
#include <QSettings>
#include <QStandardPaths>

Q_DECLARE_METATYPE(Palette)
Q_DECLARE_METATYPE(PlaylistOrder)
Q_DECLARE_METATYPE(SpotifyContext)

class Settings
{
public:
	Settings();

	QJsonObject toJson();
	void fromJson(const QJsonObject &json);
	void save();
	void load();

	typedef struct Account
	{
		QString accessToken;
		QString refreshToken;
		QString clientId;
		QString clientSecret;
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
		bool showChangelog = true;
		bool showContextInfo = true;
		bool singleClickPlay = true;
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
		QString path;
		QString username;
		bool startClient = false;
		bool globalConfig = false;
		bool alwaysStart = true;
		bool keyringPassword = false;
		int bitrate = 320;
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

	template<typename T>
	void setValue(const QJsonObject &json, const QString &key, T &value);
};