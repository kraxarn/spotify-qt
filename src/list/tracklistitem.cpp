#include "tracklistitem.hpp"

TrackListItem::TrackListItem(const QStringList &strings, const spt::Track &track, const QIcon &icon, int index)
	: QTreeWidgetItem(strings)
{
	setIcon(0, icon);

	setData(0, RoleTrackId, QString("spotify:track:%1").arg(track.id));
	setData(0, RoleArtistId, track.artistId);
	setData(0, RoleAlbumId, track.albumId);
	setData(0, RoleIndex, index);
	setData(0, RoleAddedDate, track.addedAt);

	if (track.isLocal || !track.isPlayable)
	{
		setDisabled(true);
		setToolTip(1, track.isLocal
			? "Local track"
			: "Unavailable");
	}

	if (!DateUtils::isEmpty(track.addedAt))
	{
		setToolTip(strings.length() - 1,
			QLocale().toString(track.addedAt.date()));
	}
}

bool TrackListItem::operator<(const QTreeWidgetItem &item) const
{
	auto column = treeWidget()->sortColumn();

	// Track number
	if (column == 0)
	{
		return data(0, RoleIndex).toInt() < item.data(0, RoleIndex).toInt();
	}

	// Added
	if (column == 5)
	{
		return data(0, RoleAddedDate).toDateTime() < item.data(0, RoleAddedDate).toDateTime();
	}

	return text(column) < item.text(column);
}