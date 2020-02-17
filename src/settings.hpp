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
private:
	QSettings *settings;
};