#include "settingsqml.hpp"

SettingsQml::SettingsQml(QObject *parent)
	: QObject(parent)
{
}

QJsonObject SettingsQml::getAccount()
{
	return settings.toJson()["Account"].toObject();
}

QJsonObject SettingsQml::getGeneral()
{
	return settings.toJson()["General"].toObject();
}

void SettingsQml::setGeneral(const QJsonObject &json)
{
	update("General", json);
}

QJsonObject SettingsQml::getSpotify()
{
	return settings.toJson()["Spotify"].toObject();
}

void SettingsQml::setSpotify(const QJsonObject &json)
{
	update("Spotify", json);
}

void SettingsQml::update(const QString &section, const QJsonObject &value)
{
	auto json = settings.toJson();
	json[section] = value;
	settings.fromJson(json);
	settings.save();
}

void SettingsQml::logOut(const QString &mode)
{
	if (mode == "clearAll")
		settings.removeClient();
	if (mode == "clearAll" || mode == "logOut")
		settings.removeTokens();
}