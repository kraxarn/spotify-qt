#pragma once

#ifdef USE_DBUS

#include <QDBusInterface>
#include <QCoreApplication>

class KWallet
{
public:
	explicit KWallet(const QString &username);

	auto isEnabled() -> bool;
	auto unlocked() const -> bool;
	auto unlock() -> bool;

	auto writePassword(const QString &password) -> bool;
	auto readPassword() -> QString;

private:
	QString walletName;
	int walletHandle = 0;

	QDBusInterface dbus;

	QString username;
	QString appName;

	auto getWallet() -> bool;
};

#endif
