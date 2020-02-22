#pragma once

#include <QSettings>

class Settings
{
public:
	Settings();
	virtual ~Settings();

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

	// Spotify settings

	QString sptPath();
	void setSptPath(const QString &value);

	bool sptStartClient();
	void setSptStartClient(bool value);
private:
	QSettings *settings;
};