#pragma once

#include <QSettings>

class Settings
{
public:
	Settings();
	virtual ~Settings();

	enum Palette
	{
		paletteApp   = 0,	// Default app palette
		paletteStyle = 1,	// Palette from current style
		paletteDark  = 2,	// Custom dark palette
	};

	QString fileName();

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

	bool darkTheme();
	void setDarkTheme(bool value);

	bool sptPlaybackOrder();
	void setSptPlaybackOrder(bool value);

	QList<int> hiddenSongHeaders();
	void setHiddenSongHeaders(const QList<int> &values);
	void addHiddenSongHeader(int id);
	void removeHiddenSongHeader(int id);

	// Spotify settings

	QString sptPath();
	void setSptPath(const QString &value);

	bool sptStartClient();
	void setSptStartClient(bool value);

	QString sptUser();
	void setSptUser(const QString &value);

	int sptBitrate();
	void setSptBitrate(int value);

	// Clear and log out

	void removeClient();
	void removeTokens();

private:
	QSettings *settings;
};