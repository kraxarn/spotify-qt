#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "view/search/searchtabtree.hpp"
#include "enum/column.hpp"
#include "util/tooltip.hpp"

namespace Search
{
	class Tracks: public SearchTabTree
	{
	Q_OBJECT

	public:
		Tracks(lib::spt::api &spotify, lib::cache &cache, lib::settings &settings, QWidget *parent);

		void add(const lib::spt::track &track);

	protected:
		void resizeEvent(QResizeEvent *event) override;

	private:
		/** When to show album column */
		static constexpr int albumWidthThreshold = 340;

		lib::spt::api &spotify;
		lib::cache &cache;
		Tooltip tooltip;

		void onItemDoubleClicked(QTreeWidgetItem *item, int column);
		void onContextMenu(const QPoint &pos);
		void onItemEntered(QTreeWidgetItem *item, int column);
	};
}
