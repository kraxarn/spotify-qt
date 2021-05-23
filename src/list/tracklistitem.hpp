#pragma once

#include "lib/spotify/track.hpp"
#include "role/tracklistcolumn.hpp"

#include <QIcon>
#include <QDateTime>

class TrackListItem
{
public:
	/** New track list item */
	TrackListItem(const lib::spt::track &track,
		const QIcon &icon, int index);

	/** New track list item with default icon */
	TrackListItem(const lib::spt::track &track, int index);

	/** QDateTime representation of track.added_at */
	auto addedAt() const -> QDateTime;

	/** If track can't be played */
	auto isDisabled() const -> bool;

	/** Get the underlying track */
	auto getTrack() const -> const lib::spt::track &;

	/** Get index of track in context */
	auto getIndex() const -> int;

	/** Track's ID match */
	auto operator==(const lib::spt::track &track) const -> bool;

private:
	lib::spt::track track;
	int index = -1;
	QIcon icon;

	/** Remove "The" prefix if present */
	static auto removePrefix(const QString &str) -> QString;

	/** Empty placeholder icon */
	static auto emptyIcon() -> QIcon;
};
