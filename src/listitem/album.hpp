#pragma once

#include "lib/spotify/album.hpp"

#include <QTreeWidgetItem>

namespace ListItem
{
	class Album final : public QTreeWidgetItem
	{
	public:
		Album(const lib::spt::album &album, QTreeWidget *parent);

		Album(const lib::spt::album &album, QTreeWidgetItem *parent);

	private:
		enum class Column: quint8
		{
			Name = 0,
			Year = 1,
		};

		void setAlbum(const lib::spt::album &album);

		auto operator<(const QTreeWidgetItem &item) const -> bool override;
	};
}
