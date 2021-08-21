#pragma once

#include "enum/crashrole.hpp"
#include "lib/crash/crashinfo.hpp"

#include <QListWidgetItem>
#include <QDateTime>

namespace ListItem
{
	class Crash: public QListWidgetItem
	{
	public:
		Crash(const lib::crash_info &crashInfo, QListWidget *parent);

	private:
		auto operator<(const QListWidgetItem &item) const -> bool override;
	};
}
