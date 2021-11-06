#include "dbusnotifications.hpp"

#ifdef USE_DBUS

#define SERVICE_NAME QStringLiteral("org.freedesktop.Notifications")
#define SERVICE_PATH QStringLiteral("/org/freedesktop/Notifications")

#define DEFAULT_KEY QStringLiteral("default")
#define PREVIOUS_KEY QStringLiteral("backward")
#define PAUSE_KEY QStringLiteral("pause")
#define NEXT_KEY QStringLiteral("next")

DbusNotifications::DbusNotifications(QObject *parent)
	: QObject(parent),
	dbus(QDBusConnection::sessionBus())
{
	dbus.connect(SERVICE_NAME, SERVICE_PATH, SERVICE_NAME, QStringLiteral("ActionInvoked"),
		this, SLOT(onActionInvoked(uint, QString)));
}

auto DbusNotifications::getCapabilities() -> QList<QString>
{
	if (!isConnected())
	{
		return {};
	}

	QDBusInterface interface(SERVICE_NAME, SERVICE_PATH, SERVICE_NAME, dbus);

	return interface.call("GetCapabilities")
		.arguments()
		.at(0)
		.toStringList();
}

void DbusNotifications::notify(const QString &title, const QString &message,
	const QString &imagePath, int timeout)
{
	const auto capabilities = getCapabilities();

	// Body
	QString body;
	if (!message.isEmpty())
	{
		if (capabilities.contains("body-markup"))
		{
			body = message;
		}
		else if (capabilities.contains("body"))
		{
			// Convert markup text (html) to plain text
			QTextDocument doc;
			doc.setHtml(message);
			body = doc.toPlainText();
		}
	}

	// Notify arguments
	const auto appName = QCoreApplication::applicationName();
	const auto appIcon = QStringLiteral("spotify-qt");
	const auto summary = QString(title);

	// Some notification systems also supports icons here
	const QStringList actions{
		// Clicking the notification
		DEFAULT_KEY,
		QString(),
		// Previous track
		PREVIOUS_KEY,
		QStringLiteral("Previous️"),
		// Play/Pause
		PAUSE_KEY,
		QStringLiteral("Pause"),
		// Next track
		NEXT_KEY,
		QStringLiteral("Next"),
	};

	QVariantMap hints;
	hints["suppress-sound"] = true;
	hints["image-path"] = imagePath;

	QDBusInterface interface(SERVICE_NAME, SERVICE_PATH, SERVICE_NAME, dbus);
	auto response = interface.call("Notify", appName, notificationId, appIcon, summary,
		body, actions, hints, timeout);

	if (response.type() == QDBusMessage::ErrorMessage)
	{
		lib::log::error("Failed to send notification: {}",
			response.errorMessage().toStdString());
	}
	else
	{
		auto arguments = response.arguments();
		if (!arguments.isEmpty())
		{
			bool ok;
			auto id = arguments.at(0).toUInt(&ok);
			if (ok)
			{
				notificationId = id;
			}
		}
	}
}

auto DbusNotifications::isConnected() -> bool
{
	if (dbus.isConnected())
	{
		return true;
	}

	lib::log::warn("Notification service failed: Not connected");
	return false;
}

void DbusNotifications::setOnAction(std::function<void(NotificationAction)> callback)
{
	onAction = std::move(callback);
}

void DbusNotifications::onActionInvoked(uint id, const QString &actionKey)
{
	if (id != notificationId)
	{
		lib::log::warn("Notification ID mismatch, ignoring action");
		return;
	}

	if (!onAction)
	{
		return;
	}

	if (actionKey == DEFAULT_KEY)
	{
		onAction(NotificationAction::Default);
	}
	else if (actionKey == PREVIOUS_KEY)
	{
		onAction(NotificationAction::Previous);
	}
	else if (actionKey == PAUSE_KEY)
	{
		onAction(NotificationAction::PlayPause);
	}
	else if (actionKey == NEXT_KEY)
	{
		onAction(NotificationAction::Next);
	}
	else
	{
		lib::log::warn("Unknown key action: {}", actionKey.toStdString());
	}
}

#endif
