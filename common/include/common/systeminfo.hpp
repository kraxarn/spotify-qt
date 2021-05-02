#pragma once

#include "common/util/systemutils.hpp"

#include <QString>
#include <QMap>

class SystemInfo
{
public:
	SystemInfo();

	void add(const QString &key, const QString &value);

	auto toText() -> QString;
	auto toHtml() -> QString;


private:
	QMap<QString, QString> info;
};
