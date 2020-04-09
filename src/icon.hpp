#pragma once

#include "settings.hpp"

#include <QIcon>
#include <QString>
#include <QDebug>

class Icon
{
public:
	static QIcon get(const QString &name);

private:
	Icon();
	static QString theme;
};