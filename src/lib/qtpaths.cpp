#include "qtpaths.hpp"

QtPaths::QtPaths(QObject *parent)
	: QObject(parent)
{
	configFile = QStringLiteral("%1.json").arg(QStandardPaths::writableLocation
		(QStandardPaths::AppConfigLocation)).toStdString();

	cachePath = QStandardPaths::writableLocation
		(QStandardPaths::CacheLocation).toStdString();
}

auto QtPaths::config_file() const -> ghc::filesystem::path
{
	return configFile;
}

auto QtPaths::cache() const -> ghc::filesystem::path
{
	return cachePath;
}
