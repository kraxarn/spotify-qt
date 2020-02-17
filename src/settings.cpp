#include "settings.hpp"

Settings::Settings()
{
	settings = new QSettings();
}

Settings::~Settings()
{
	delete settings;
}

QString Settings::accessToken()
{
	return settings->value("AccessToken").toString();
}

void Settings::setAccessToken(QString &value)
{
	settings->setValue("AccessToken", value);
}

QString Settings::refreshToken()
{
	return settings->value("RefreshToken").toString();
}

void Settings::setRefreshToken(QString &value)
{
	settings->setValue("RefreshToken", value);
}
