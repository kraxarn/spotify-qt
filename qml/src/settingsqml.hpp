#pragma once

#include "common.hpp"

class SettingsQml: public QObject
{
Q_OBJECT
	Q_PROPERTY(QJsonObject settings READ getSettings WRITE setSettings NOTIFY settingsChanged)
	QML_ELEMENT

public:
	explicit SettingsQml(QObject *parent = nullptr);

	QJsonObject getSettings();
	void setSettings(const QJsonObject &value);

signals:
	void settingsChanged();

private:
	Settings settings;
};