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
		TracksCache(lib::cache &cache, QWidget *parent);

		void setPlaylistId(const std::string &value);

	protected:
		void showEvent(QShowEvent *event) override;

	private:
		static constexpr int width = 500;
		static constexpr int height = 300;

		std::string playlistId;
		QTreeWidget *tree = nullptr;
		lib::cache &cache;

		void addTrack(const lib::spt::track &track);
	};
}
