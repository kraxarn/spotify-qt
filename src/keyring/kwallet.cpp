#include "kwallet.hpp"

#ifdef USE_DBUS

KWallet::KWallet(const QString &username)
	: username(QString(username)),
	dbus("org.kde.kwalletd5", "/modules/kwalletd5",
		"org.kde.KWallet", QDBusConnection::sessionBus()),
	appName(QCoreApplication::applicationName())
{
}

auto KWallet::isEnabled() -> bool
{
	auto call = dbus.call(QDBus::Block, "isEnabled");
	if (call.type() != QDBusMessage::ReplyMessage)
	{
		return false;
	}

	return call.arguments().at(0).toBool();
}

auto KWallet::unlocked() const -> bool
{
	return !walletName.isEmpty() && walletHandle > 0;
}

auto KWallet::getWallet() -> bool
{
	// Wallet name
	walletName = dbus
		.call(QDBus::Block, "localWallet")
		.arguments().at(0).toString();

	if (walletName.isEmpty())
	{
		return false;
	}

	// Wallet handle
	walletHandle = dbus
		.callWithArgumentList(QDBus::Block, "open", {
			walletName,
			(qlonglong) 0,
			appName
		}).arguments().at(0).toInt();

	return walletHandle > 0;
}

auto KWallet::unlock() -> bool
{
	if (unlocked())
	{
		return true;
	}

	if (!isEnabled())
	{
		return false;
	}

	return getWallet();
}

auto KWallet::writePassword(const QString &password) -> bool
{
	if (!unlocked() && !unlock())
	{
		return false;
	}

	dbus.callWithArgumentList(QDBus::Block, "writePassword", {
		walletHandle, appName, username, password, appName
	});

	return true;
}

auto KWallet::readPassword() -> QString
{
	if (!unlocked() && !unlock())
	{
		return {};
	}

	return dbus
		.callWithArgumentList(QDBus::Block, "readPassword", {
			walletHandle, appName, username, appName
		}).arguments().at(0).toString();
}

#endif
