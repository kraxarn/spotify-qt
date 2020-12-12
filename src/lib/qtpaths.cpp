#include "qtpaths.hpp"

#include <QDebug>

QtPaths::QtPaths(QObject *parent)
	: QObject(parent)
{
	configFile = QString("%1.json")
		.arg(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
	cachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
}

std::string QtPaths::config_file() const
{
	return configFile.toStdString();
}

std::string QtPaths::cache() const
{
	return cachePath.toStdString();
}