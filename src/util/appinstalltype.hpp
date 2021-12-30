#pragma once

#include "enum/installtype.hpp"

#include <QString>

class AppInstallType
{
public:
	/**
	 * Get current install type
	 */
	static auto get() -> InstallType;

	/**
	 * Get current serialized install type
	 */
	static auto getString() -> QString;

	/**
	 * Set a new install type, cannot be changed once set
	 * @return Change was successful
	 */
	static auto set(InstallType installType) -> bool;

	/**
	 * Get install type from path
	 */
	static auto getInstallType(const QString &path) -> InstallType;

private:
	AppInstallType() = default;

	static auto toString(InstallType installType) -> QString;

	static InstallType installType;
};


