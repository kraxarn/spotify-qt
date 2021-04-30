#include "playlistlistitem.hpp"

PlaylistListItem::PlaylistListItem(const lib::spt::playlist &playlist,
	int index, QListWidget *parent)
	: index(index),
	ListWidgetItem<lib::spt::playlist>(playlist, parent)
{
	QTextDocument doc;
	doc.setHtml(QString::fromStdString(playlist.description));
	setToolTip(doc.toPlainText());
}

auto PlaylistListItem::getIndex() const -> int
{
	return index;
}
