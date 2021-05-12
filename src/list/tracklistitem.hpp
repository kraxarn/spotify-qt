#pragma once

#include "enum/datarole.hpp"
#include "lib/spotify/track.hpp"
#include "util/dateutils.hpp"
#include "util/utils.hpp"
#include "lib/settings.hpp"
#include "lib/spotify/entity.hpp"
#include "metatypes.hpp"

#include <QTreeWidgetItem>

class TrackListItem: public QTreeWidgetItem
{
public:
	TrackListItem(const QStringList &strings, const lib::spt::track &track, const QIcon &icon,
		int index);

private:
	auto operator<(const QTreeWidgetItem &item) const -> bool override;

	static auto removePrefix(const QString &str) -> QString;
};
