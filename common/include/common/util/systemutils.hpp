#pragma once

#include <QString>

class SystemUtils
{
public:
	/**
	 * Get environmental variable
	 */
	static auto env(const char *name) -> QString;

	/**
	 * Environmental variable is set in system
	 */
	static auto hasEnv(const char *name) -> bool;

private:
	SystemUtils() = default;
};
