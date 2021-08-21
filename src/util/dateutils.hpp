#pragma once

#include <QString>
#include <QDateTime>

class DateUtils
{
public:
	static auto toRelative(const QDateTime &date) -> QString;
	static auto toRelative(const std::string &date) -> QString;
	static auto isEmpty(const QDateTime &date) -> bool;
	static auto fromIso(const QString &date) -> QDateTime;
	static auto fromIso(const std::string &date) -> QDateTime;

private:
	DateUtils() = default;
};