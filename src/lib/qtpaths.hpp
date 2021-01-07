#pragma once

#include "lib/paths/paths.hpp"

#include <QObject>
#include <QStandardPaths>

class QtPaths: public QObject, public lib::paths
{
public:
	explicit QtPaths(QObject *parent);

	std::string config_file() const override;
	std::string cache() const override;

private:
	std::string configFile;
	std::string cachePath;
};