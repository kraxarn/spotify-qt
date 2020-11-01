#ifdef USE_DBUS

#include "kwallet.hpp"

#include <utility>

KWallet::KWallet(QString username)
	: walletName(),
	walletHandle(0),
	username(std::move(username)),
	dbus("org.kde.kwalletd5", "/modules/kwalletd5",
		"org.kde.KWallet", QDBusConnection::sessionBus())
{
	appName = QCoreApplication::applicationName();
}

bool KWallet::isEnabled()
{
	auto call = dbus.call(QDBus::Block, "isEnabled");
	if (call.type() != QDBusMessage::ReplyMessage)
		return false;

	return call.arguments().at(0).toBool();
}

bool KWallet::unlocked() const
{
	return !walletName.isEmpty() && walletHandle > 0;
}

bool KWallet::getWallet()
{
	// Wallet name
	walletName = dbus
		.call(QDBus::Block, "localWallet")
		.arguments().at(0).toString();

	if (walletName.isEmpty())
		return false;

	// Wallet handle
	walletHandle = dbus
		.callWithArgumentList(QDBus::Block, "open", {
			walletName,
			(qlonglong) 0,
			appName
		}).arguments().at(0).toInt();

	return walletHandle > 0;
}

bool KWallet::unlock()
{
	if (unlocked())
		return true;

	if (!isEnabled())
		return false;

	return getWallet();
}

bool KWallet::writePassword(const QString &password)
{
	if (!unlocked() && !unlock())
		return false;

	dbus.callWithArgumentList(QDBus::Block, "writePassword", {
		walletHandle, appName, username, password, appName
	});

	return true;
}

QString KWallet::readPassword()
{
	if (!unlocked() && !unlock())
		return QString();

	return dbus
		.callWithArgumentList(QDBus::Block, "readPassword", {
			walletHandle, appName, username, appName
		}).arguments().at(0).toString();
}

#endif