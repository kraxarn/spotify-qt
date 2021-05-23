#include "tracklistitem.hpp"

TrackListItem::TrackListItem(const lib::spt::track &track,
	const QIcon &icon, int index)
	: track(track),
	index(index),
	icon(icon)
{
}

TrackListItem::TrackListItem(const lib::spt::track &track, int index)
	: TrackListItem(track, emptyIcon(), index)
{
}

//auto TrackListItem::operator<(const QTreeWidgetItem &item) const -> bool
//{
//	auto column = treeWidget()->sortColumn();
//
//	// Track number
//	if (column == 0)
//	{
//		return data(0, RoleIndex).toInt() < item.data(0, RoleIndex).toInt();
//	}
//
//	// Length
//	if (column == 4)
//	{
//		return data(0, RoleLength).toInt() < item.data(0, RoleLength).toInt();
//	}
//
//	// Added
//	if (column == 5)
//	{
//		return data(0, RoleAddedDate).toDateTime() < item.data(0, RoleAddedDate).toDateTime();
//	}
//
//	return removePrefix(text(column))
//		.compare(removePrefix(item.text(column)),
//			Qt::CaseInsensitive) < 0;
//}

auto TrackListItem::removePrefix(const QString &str) -> QString
{
	return str.startsWith("The ", Qt::CaseInsensitive)
		? str.right(str.length() - 4)
		: str;
}

auto TrackListItem::addedAt() const -> QDateTime
{
	auto date = QString::fromStdString(track.added_at);
	return QDateTime::fromString(date, Qt::DateFormat::ISODate);
}

auto TrackListItem::isDisabled() const -> bool
{
	return track.is_local || !track.is_playable;
}

auto TrackListItem::isValid() const -> bool
{
	return track.is_valid();
}

auto TrackListItem::emptyIcon() -> QIcon
{
	constexpr int emptyPixmapSize = 64;
	QPixmap emptyPixmap(emptyPixmapSize, emptyPixmapSize);
	emptyPixmap.fill(Qt::transparent);
	return QIcon(emptyPixmap);
}

auto TrackListItem::getTrack() const -> const lib::spt::track &
{
	return track;
}

auto TrackListItem::getIndex() const -> int
{
	return index;
}

auto TrackListItem::operator==(const lib::spt::track &item) const -> bool
{
	return item.id == track.id;
}
