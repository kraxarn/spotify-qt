#pragma once

#include <QString>
#include <QDateTime>

class DateUtils
{
public:
	static QString toRelative(const QDateTime &date);

	static bool isEmpty(const QDateTime &date);

	static QDateTime fromIso(const QString &date);

	static QDateTime fromIso(const std::string &date);

private:
	DateUtils() = default;
};