#pragma once

#include "lib/crash/crashinfo.hpp"

#include <QListWidgetItem>
#include <QDateTime>

namespace ListItem
{
	class Crash: public QListWidgetItem
	{
	public:
		Crash(const lib::crash_info &crashInfo, QListWidget *parent);

		enum class Role: int
		{
			StackTrace = 0x100,
			Timestamp = 0x101,
		};

	private:
		auto operator<(const QListWidgetItem &item) const -> bool override;
	};
}
