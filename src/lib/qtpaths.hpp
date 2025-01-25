#pragma once

#include "lib/paths/paths.hpp"

#include <QObject>
#include <QStandardPaths>

class QtPaths: public QObject, public lib::paths
{
public:
	explicit QtPaths(QObject *parent);

	auto config_file() const -> std::filesystem::path override;
	auto cache() const -> std::filesystem::path override;

private:
	std::filesystem::path configFile;
	std::filesystem::path cachePath;
};
