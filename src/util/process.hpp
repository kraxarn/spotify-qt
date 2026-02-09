#pragma once

#include "lib/result.hpp"

#include <QStringList>

class Process
{
public:
	static auto exec(const QString &path, const QStringList &args) -> Result<QString>;

private:
	Process() = default;
};
