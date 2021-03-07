#include "tracklistitem.hpp"

TrackListItem::TrackListItem(const QStringList &strings,
	const spt::Track &track,
	const QIcon &icon,
	int index)
	: QTreeWidgetItem(strings)
{
	setIcon(0, icon);

	setData(0, RoleTrackId, QString("spotify:track:%1").arg(track.id));
	setData(0, RoleArtistId, track.artistId);
	setData(0, RoleAlbumId, track.albumId);
	setData(0, RoleIndex, index);
	setData(0, RoleAddedDate, track.addedAt);
	setData(0, RoleLength, track.duration);

	if (track.isLocal || !track.isPlayable)
	{
		setDisabled(true);
		setToolTip(1, track.isLocal
			? "Local track"
			: "Unavailable");
	}

	// Title, artist album
	for (auto i = 1; i < strings.length() - 2; i++)
		setToolTip(i, strings.at(i));

	// Length
	auto length = strings.at(strings.length() - 2).split(':');
	setToolTip(strings.length() - 2,
		QString("%1m %2s (%3s total)")
			.arg(length.at(0))
			.arg(length.at(1))
			.arg(track.duration / 1000));

	// Added
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

	// Length
	if (column == 4)
	{
		return data(0, RoleLength).toInt() < item.data(0, RoleLength).toInt();
	}

	// Added
	if (column == 5)
	{
		return data(0, RoleAddedDate).toDateTime() < item.data(0, RoleAddedDate).toDateTime();
	}

	return text(column) < item.text(column);
}