#pragma once

#include "../src/settings.hpp"

#include <QObject>

class QmlManager: QObject
{
	Q_OBJECT


public:
	explicit QmlManager(Settings &settings);

	void load(const QString &url);

	bool setup();
	void main();

private:
	QObject *appEngine;
	Settings &settings;

	void defineTypes();
};


