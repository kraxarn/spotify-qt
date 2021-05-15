#include "lib/qt/util/systemutils.hpp"

auto lib::qt::util::system::env(const char *name) -> QString
{
	if (!has_env(name))
	{
		return QString();
	}

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
	return qEnvironmentVariable(name);
#else
	return QString::fromLocal8Bit(qgetenv(name));
#endif
}

auto lib::qt::util::system::has_env(const char *name) -> bool
{
	return qEnvironmentVariableIsSet(name);
}
