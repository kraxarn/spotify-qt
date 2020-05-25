#pragma once

#include "mainwindow.hpp"
#include "settings.hpp"

#include <QIcon>
#include <QString>
#include <QDebug>

class Icon
{
public:
	static QIcon get(const QString &name);
	static Settings *settings;

private:
	Icon();
	static QString theme;
	static QVariant useFallbackIcons;
};