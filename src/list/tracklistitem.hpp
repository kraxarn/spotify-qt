#pragma once

#include "../enum/datarole.hpp"
#include "../spotify/track.hpp"
#include "../util/dateutils.hpp"
#include "../util/utils.hpp"
#include "lib/settings.hpp"

#include <QTreeWidgetItem>

class TrackListItem: public QTreeWidgetItem
{
public:
	TrackListItem(const QStringList &strings, const spt::Track &track, const QIcon &icon, int index);

private:
	bool operator<(const QTreeWidgetItem &item) const override;
};
