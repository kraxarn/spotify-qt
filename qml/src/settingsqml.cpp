#include "settingsqml.hpp"

SettingsQml::SettingsQml(QObject *parent)
	: QObject(parent)
{
}

QJsonObject SettingsQml::getSettings()
{
	return settings.toJson();
}

void SettingsQml::setSettings(const QJsonObject &value)
{
	settings.fromJson(value);
}