#pragma once

#include "../mainwindow.hpp"
#include "../settings.hpp"

#include <QDebug>
#include <QIcon>
#include <QString>

class Icon
{
public:
	static QIcon get(const QString &name);
	static bool useFallbackIcons;

private:
	Icon() = default;
	static QString theme;
};