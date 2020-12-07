#pragma once

#include <QString>
#include <QDateTime>

class DateUtils
{
public:
	static QString toRelative(const QDateTime &date);

	static bool isEmpty(const QDateTime &date);

private:
	DateUtils() = default;
};