#include "tracklistitem.hpp"

TrackListItem::TrackListItem(const QStringList &strings, const lib::spt::track &track,
	const QIcon &icon, int index)
	: index(index),
	track(track),
	QTreeWidgetItem(strings)
{
	setIcon(0, icon);

	addedAt = QDateTime::fromString(QString::fromStdString(track.added_at),
		Qt::DateFormat::ISODate);

	if (track.is_local || !track.is_playable)
	{
		setDisabled(true);
		setToolTip(1, track.is_local
			? "Local track"
			: "Unavailable");
	}

	// Artist
	setToolTip(2,
		QString::fromStdString(lib::spt::entity::combine_names(track.artists, "\n")));

	// Title, album
	for (auto i = 1; i < strings.length() - 2; i++)
	{
		if (toolTip(i).isEmpty())
		{
			setToolTip(i, strings.at(i));
		}
	}

	// Length
	auto lengths = strings.at(strings.length() - 2).split(':');
	if (lengths.length() >= 2)
	{
		setToolTip(strings.length() - 2,
			QString("%1m %2s (%3s total)")
				.arg(lengths.at(0), lengths.at(1))
				.arg(track.duration / 1000));
	}

	// Added
	if (!DateUtils::isEmpty(addedAt))
	{
		setToolTip(strings.length() - 1,
			QLocale().toString(addedAt.date()));
	}
}

auto TrackListItem::operator<(const QTreeWidgetItem &item) const -> bool
{
	const auto *trackItem = dynamic_cast<const TrackListItem*>(&item);
	if (trackItem == nullptr)
	{
		return false;
	}
	auto column = treeWidget()->sortColumn();

	// Track number
	if (column == indexTrackNumber)
	{
		return index < trackItem->getIndex();
	}

	// Length
	if (column == indexLength)
	{
		return track.duration < trackItem->getTrack().duration;
	}

	// Added
	if (column == indexAdded)
	{
		return addedAt < trackItem->getAddedAt();
	}

	return text(column) < item.text(column);
}

auto TrackListItem::getIndex() const -> int
{
	return index;
}

auto TrackListItem::getTrack() const -> const lib::spt::track &
{
	return track;
}

auto TrackListItem::getAddedAt() const -> const QDateTime &
{
	return addedAt;
}
