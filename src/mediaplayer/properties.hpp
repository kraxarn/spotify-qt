#pragma once

#include <QDBusAbstractAdaptor>
#include <QMap>
#include <QDebug>

namespace mp
{
	class Properties : public QDBusAbstractAdaptor
	{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.freedesktop.DBus.Properties")

	public:
		explicit Properties(QObject *parent);
		void updateMetadata(const QMap<QString, QVariant> &metadata);

	signals:
		void PropertiesChanged(QString interfaceName, QMap<QString, QVariant> changedProperties,
			QStringList invalidatedProperties);
	};
}