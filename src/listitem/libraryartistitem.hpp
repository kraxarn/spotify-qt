#pragma once

#include "lib/spotify/artist.hpp"
#include "treewidgetitem.hpp"

class LibraryArtistItem: public TreeWidgetItem<lib::spt::artist>
{
public:
	LibraryArtistItem(const lib::spt::artist &artist, QTreeWidgetItem *parent)
		: TreeWidgetItem<lib::spt::artist>(artist, parent)
	{
		setText(0, QString::fromStdString(artist.name));
	}
};
