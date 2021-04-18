#pragma once

#include <QStringList>

#include <string>
#include <vector>

/**
 * Qt type specific utilities
 */
class QtUtils
{
public:
	/**
	 * C++ vector<string> to Qt QStringList
	 */
	static auto toStringList(const std::vector<std::string> &vec) -> QStringList;
};
