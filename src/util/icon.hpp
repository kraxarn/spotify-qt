#pragma once

#include "lib/settings.hpp"
#include "utils.hpp"

#include <QIcon>
#include <QString>

class Icon
{
public:
	static auto get(const QString &name) -> QIcon;
	static bool useFallbackIcons;

private:
	Icon() = default;
};
