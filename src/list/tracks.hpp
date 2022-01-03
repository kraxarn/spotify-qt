#pragma once

#include "lib/cache.hpp"
#include "lib/set.hpp"
#include "spotify/current.hpp"
#include "menu/track.hpp"
#include "enum/column.hpp"

#include <QListWidget>
#include <QTreeWidgetItem>

namespace List
{
	class Tracks: public QTreeWidget
	{
	Q_OBJECT

	public:
		Tracks(lib::spt::api &spotify, lib::settings &settings, lib::cache &cache,
			QWidget *parent);

		void updateResizeMode(lib::resize_mode mode);
		void setPlayingTrackItem(QTreeWidgetItem *item);
		void setPlayingTrackItem(const std::string &itemId);

		/**
		 * Load tracks directly, without cache, but select an item
		 */
		void load(const std::vector<lib::spt::track> &tracks, const std::string &selectedId);

		/**
		 * Load tracks directly, without cache,
		 * but select an item and provide a fallback added date
		 */
		void load(const std::vector<lib::spt::track> &tracks, const std::string &selectedId,
			const std::string &addedAt);

		/**
		 * Load tracks directly, without cache
		 */
		void load(const std::vector<lib::spt::track> &tracks);

		/**
		 * Load playlist first from cache, then refresh it
		 */
		void load(const lib::spt::playlist &playlist);

		/** Force refresh tracks in playlist */
		void refreshPlaylist(const lib::spt::playlist &playlist);

		/**
		 * Load album first from cache, then refresh it
		 * @param album Album to load
		 * @param trackId Track to highlight
		 */
		void load(const lib::spt::album &album, const std::string &trackId = std::string());

	protected:
		void resizeEvent(QResizeEvent *event) override;

	private:
		void menu(const QPoint &pos);
		void clicked(QTreeWidgetItem *item, int column);
		void headerMenu(const QPoint &pos);
		void resizeHeaders(const QSize &newSize);
		auto getCurrent() -> const spt::Current &;
		auto getAddedText(const std::string &date) const -> QString;

		// lib
		lib::settings &settings;
		lib::cache &cache;
		lib::spt::api &spotify;
		// std
		std::unordered_map<std::string, QTreeWidgetItem *> trackItems;
		// qt
		QTreeWidgetItem *playingTrackItem = nullptr;
		QIcon emptyIcon;
	};
}
