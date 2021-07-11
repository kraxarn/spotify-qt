#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "searchtab/searchtabtree.hpp"
#include "enum/column.hpp"

namespace SearchTab
{
	class Tracks: public SearchTabTree
	{
	public:
		Tracks(lib::spt::api &spotify, lib::cache &cache, QWidget *parent);

		void add(const lib::spt::track &track);

	protected:
		void resizeEvent(QResizeEvent *event) override;

	private:
		/** When to show album column */
		static constexpr int albumWidthThreshold = 340;

		lib::spt::api &spotify;
		lib::cache &cache;

		void onItemActivated(QTreeWidgetItem *item, int column);
		void onContextMenu(const QPoint &pos);
	};
}
