#include "util/appinstalltype.hpp"
#include "lib/log.hpp"

InstallType AppInstallType::installType = InstallType::Unknown;

auto AppInstallType::get() -> InstallType
{
	return installType;
}

auto AppInstallType::getString() -> QString
{
	return toString(get());
}

auto AppInstallType::set(InstallType value) -> bool
{
	if (installType == InstallType::Unknown || installType == value)
	{
		installType = value;
		return true;
	}

	lib::log::warn("Attempted to change install type to {} (is {}), ignored",
		toString(value).toStdString(),
		toString(installType).toStdString());

	return false;
}

auto AppInstallType::getInstallType(const QString &path) -> InstallType
{
	if (path.startsWith(QStringLiteral("/home/")))
	{
		return InstallType::User;
	}

	if (path.startsWith(QStringLiteral("/usr/")))
	{
		return InstallType::System;
	}

	if (path.startsWith(QStringLiteral("/snap/")))
	{
		return InstallType::Snap;
	}

	return InstallType::Unknown;
}

auto AppInstallType::toString(InstallType value) -> QString
{
	switch (value)
	{
		case InstallType::Unknown:
			return QStringLiteral("Unknown");

		case InstallType::User:
			return QStringLiteral("User");

		case InstallType::System:
			return QStringLiteral("System");

		case InstallType::Snap:
			return QStringLiteral("Snap");

		default:
			return {};
	}
}
