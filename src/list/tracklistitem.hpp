#pragma once

#include "lib/spotify/track.hpp"
#include "../util/dateutils.hpp"
#include "../util/utils.hpp"
#include "lib/settings.hpp"
#include "lib/spotify/entity.hpp"

#include <QTreeWidgetItem>

class TrackListItem: public QTreeWidgetItem
{
public:
	TrackListItem(const QStringList &strings, const lib::spt::track &track, const QIcon &icon,
		int index);

	auto getIndex() const -> int;
	auto getTrack() const -> const lib::spt::track &;
	auto getAddedAt() const -> const QDateTime &;

private:
	auto operator<(const QTreeWidgetItem &item) const -> bool override;

	int index = 0;
	const lib::spt::track &track;
	QDateTime addedAt;
	int length = 0;

	static constexpr int indexTrackNumber = 0;
	static constexpr int indexLength = 4;
	static constexpr int indexAdded = 5;
};
