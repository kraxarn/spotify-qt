#pragma once

#include <QString>
#include <QJsonObject>
#include <QStringList>
#include <QMap>

namespace spt
{
	class AudioFeatures
	{
	public:
		explicit AudioFeatures(const QJsonObject &json);
		QMap<QString, QString> values;
	private:
		static QString formatStat(double value, const QStringList &stages);
		static QString formatStat(double value, const QString &low, const QString &high);
		static QString formatStat(double value, const QString &measurement);
		static QString formatVolume(double value);
		static QString parseKey(int key);
	};
}