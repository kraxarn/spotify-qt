#include "qtutils.hpp"

auto QtUtils::toStringList(const std::vector<std::string> &vec) -> QStringList
{
	QStringList list;
	list.reserve((int) vec.size());

	for (const auto &str : vec)
	{
		list << QString::fromStdString(str);
	}
	return list;
}
