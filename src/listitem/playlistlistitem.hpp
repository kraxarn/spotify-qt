#pragma once

#include "listitem/listwidgetitem.hpp"
#include "lib/spotify/playlist.hpp"

#include <QTextDocument>

class PlaylistListItem: public ListWidgetItem<lib::spt::playlist>
{
public:
	PlaylistListItem(const lib::spt::playlist &playlist, int index, QListWidget *parent);

	auto getIndex() const -> int;

private:
	int index = 0;
};


