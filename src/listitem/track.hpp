#pragma once

#include "enum/datarole.hpp"
#include "enum/column.hpp"
#include "lib/spotify/track.hpp"
#include "util/dateutils.hpp"
#include "lib/settings.hpp"
#include "lib/spotify/entity.hpp"
#include "metatypes.hpp"

#include <QTreeWidgetItem>

namespace ListItem
{
	class Track: public QTreeWidgetItem
	{
	public:
		Track(const QStringList &strings, const lib::spt::track &track,
			const QIcon &icon, int index);

	private:
		auto operator<(const QTreeWidgetItem &item) const -> bool override;

		static auto removePrefix(const QString &str) -> QString;
	};
}
