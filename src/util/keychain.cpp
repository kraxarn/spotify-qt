#ifdef USE_KEYCHAIN

#include "util/keychain.hpp"
#include "thirdparty/keychain.h"
#include "lib/log.hpp"

auto Keychain::getPassword(const QString &user) -> QString
{
	keychain::Error error;
	const auto password = keychain::getPassword(PKG_NAME, APP_NAME, user.toStdString(), error);

	if (error)
	{
		lib::log::warn("Failed to get password: {}", error.message);
		return {};
	}

	return QString::fromStdString(password);
}

auto Keychain::setPassword(const QString &user, const QString &password) -> bool
{
	keychain::Error error;
	keychain::setPassword(PKG_NAME, APP_NAME, user.toStdString(), password.toStdString(), error);

	if (error)
	{
		lib::log::warn("Failed to set password: {}", error.message);
		return false;
	}

	return true;
}

auto Keychain::clearPassword(const QString &user) -> bool
{
	keychain::Error error;
	keychain::deletePassword(PKG_NAME, APP_NAME, user.toStdString(), error);

	if (error)
	{
		lib::log::warn("Failed to clear password: {}", error.message);
		return false;
	}

	return true;
}

#endif
