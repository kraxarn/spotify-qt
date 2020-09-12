#pragma once

#include "common.hpp"

class SettingsQml: public QObject
{
Q_OBJECT
	Q_PROPERTY(QJsonObject account READ getAccount NOTIFY accountChanged)
	Q_PROPERTY(QJsonObject general READ getGeneral WRITE setGeneral NOTIFY generalChanged)
	Q_PROPERTY(QJsonObject spotify READ getSpotify WRITE setSpotify NOTIFY spotifyChanged)
	QML_ELEMENT

public:
	explicit SettingsQml(QObject *parent = nullptr);

	Q_INVOKABLE void logOut(const QString &mode);

	QJsonObject getAccount();

	QJsonObject getGeneral();
	void setGeneral(const QJsonObject &json);

	QJsonObject getSpotify();
	void setSpotify(const QJsonObject &json);

signals:
	void accountChanged();
	void generalChanged();
	void spotifyChanged();

private:
	Settings settings;

	void update(const QString &section, const QJsonObject &value);
};