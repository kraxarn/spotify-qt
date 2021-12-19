#pragma once

#ifdef USE_DBUS

#include "lib/log.hpp"

#include "enum/notificationaction.hpp"

#include <utility>

#include <QList>
#include <QTextDocument>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusInterface>
#include <QCoreApplication>

/**
 * Notifications using d-bus through the
 * org.freedesktop.Notifications interface
 */
class DbusNotifications: public QObject
{
Q_OBJECT

public:
	explicit DbusNotifications(QObject *parent);

	auto getCapabilities() -> QList<QString>;

	void notify(const QString &title, const QString &message,
		const QString &imagePath, int timeout);

private:
	QDBusConnection dbus;
	uint notificationId = 0;

	std::function<void(NotificationAction)> onAction;

	auto isConnected() -> bool;
};

#endif
