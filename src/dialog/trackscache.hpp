#pragma once

#include "lib/cache.hpp"
#include "dialog/base.hpp"

#include <QTreeWidget>

namespace Dialog
{
	class TracksCache: public Base
	{
	Q_OBJECT

	public:
		explicit TracksCache(QWidget *parent);

		void load(const std::vector<lib::spt::track> &tracks);
		void loadAllTracks(const lib::cache &cache);

	private:
		static constexpr int width = 500;
		static constexpr int height = 300;

		QTreeWidget *tree = nullptr;

		void addTrack(const lib::spt::track &track);
	};
}
