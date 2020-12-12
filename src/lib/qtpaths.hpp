#pragma once

#include "lib/paths/paths.hpp"

#include <QObject>
#include <QStandardPaths>

class QtPaths: public QObject, public lib::paths
{
public:
	explicit QtPaths(QObject *parent);

	[[nodiscard]]
	std::string config_file() const override;

	[[nodiscard]]
	std::string cache() const override;

private:
	QString configFile;
	QString cachePath;
};