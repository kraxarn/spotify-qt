#pragma once

#include "lib/spotify/artist.hpp"

#include <QListWidget>

namespace SearchTab
{
	class Artists: public QListWidget
	{
	public:
		explicit Artists(QWidget *parent);

		void add(const lib::spt::artist &artist);

	private:
		void onItemClicked(QListWidgetItem *item);
	};
}
