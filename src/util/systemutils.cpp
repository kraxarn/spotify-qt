#include "systemutils.hpp"

auto SystemUtils::env(const char *name) -> QString
{
	if (!hasEnv(name))
	{
		return QString();
	}

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
	return qEnvironmentVariable(name);
#else
	return QString::fromLocal8Bit(qgetenv(name));
#endif
}

auto SystemUtils::hasEnv(const char *name) -> bool
{
	return qEnvironmentVariableIsSet(name);
}
