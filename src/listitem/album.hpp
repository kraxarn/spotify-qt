#pragma once

#include "lib/spotify/album.hpp"

#include <QTreeWidgetItem>

namespace ListItem
{
	class Album final: public QTreeWidgetItem
	{
	public:
		Album(const lib::spt::album &album, QTreeWidgetItem *parent);

	private:
		enum class Column: quint8
		{
			Name = 0,
			Year = 1,
		};

		auto operator<(const QTreeWidgetItem &item) const -> bool override;
	};
}
