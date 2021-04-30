#pragma once

#include "lib/spotify/album.hpp"
#include "treewidgetitem.hpp"

class LibraryAlbumItem: public TreeWidgetItem<lib::spt::album>
{
public:
	LibraryAlbumItem(const lib::spt::album &album, QTreeWidget *parent)
		: TreeWidgetItem<lib::spt::album>(album, parent)
	{
		setText(0, QString::fromStdString(album.name));
	}
};
