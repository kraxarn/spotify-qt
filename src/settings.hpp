#pragma once

#include <QSettings>

class Settings
{
public:
	Settings();
	virtual ~Settings();

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

	bool stylePalette();
	void setStylePalette(bool value);

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