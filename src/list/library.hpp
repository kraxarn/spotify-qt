#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include "util/tree.hpp"
#include "listitem/library.hpp"
#include "util/tooltip.hpp"

#include <QTreeWidget>
#include <QHeaderView>

namespace List
{
	class Library: public QTreeWidget
	{
	Q_OBJECT

	public:
		Library(lib::spt::api &spotify, lib::cache &cache, const HttpClient &httpClient,
			lib::settings &settings, QWidget *parent);

		void load(QTreeWidgetItem *item);

	private:
		lib::spt::api &spotify;
		lib::cache &cache;
		Tooltip tooltip;

		static constexpr const int dataRole = 0x100;

		void onClicked(QTreeWidgetItem *item, int column);
		void onDoubleClicked(QTreeWidgetItem *item, int column);
		void onExpanded(QTreeWidgetItem *item);
		void onMenuRequested(const QPoint &pos);
		void onItemEntered(QTreeWidgetItem *item, int column);

		void tracksLoaded(const lib::spt::entity &entity, const std::vector<lib::spt::track> &tracks);
		static void itemsLoaded(std::vector<ListItem::Library> &items, QTreeWidgetItem *item);
	};
}
