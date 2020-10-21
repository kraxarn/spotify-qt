#pragma once

#ifdef WITH_DBUS
#include <QDBusInterface>
#endif
#include <QCoreApplication>

class KWallet
{
private:
	QString walletName;
	int walletHandle;
#if WITH_DBUS
	QDBusInterface dbus;
#endif
	QString username;
	QString appName;

	bool getWallet();

public:
	explicit KWallet(QString username);

	bool isEnabled();
	bool unlocked() const;
	bool unlock();

	bool writePassword(const QString &password);
	QString readPassword();
};
