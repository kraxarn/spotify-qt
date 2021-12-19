#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include "view/search/searchtabtree.hpp"
#include "enum/datarole.hpp"
#include "menu/show.hpp"

#include <QListWidget>
#include <QTextDocument>

namespace Search
{
	class Shows: public QListWidget
	{
	Q_OBJECT

	public:
		Shows(lib::spt::api &spotify, QWidget *parent);

		void add(const lib::spt::show &show);

	private:
		lib::spt::api &spotify;

		void onItemClicked(QListWidgetItem *item);
		void onContextMenu(const QPoint &pos);
	};
}
