#pragma once

#include <QDBusInterface>
#include <QCoreApplication>

class KWallet
{
private:
	QString walletName;
	int walletHandle;
	QDBusInterface dbus;
	QString username;
	QString appName;

	bool getWallet();

public:
	explicit KWallet(QString username);

	bool isValid() const;
	bool unlocked() const;
	bool unlock();

	bool writePassword(const QString &password);
	QString readPassword();
};