#include "qtpaths.hpp"

QtPaths::QtPaths(QObject *parent)
	: QObject(parent)
{
	configFile = QString("%1.json").arg(QStandardPaths::writableLocation
		(QStandardPaths::AppConfigLocation)).toStdString();

	cachePath = QStandardPaths::writableLocation
		(QStandardPaths::CacheLocation).toStdString();
}

std::string QtPaths::config_file() const
{
	return configFile;
}

std::string QtPaths::cache() const
{
	return cachePath;
}