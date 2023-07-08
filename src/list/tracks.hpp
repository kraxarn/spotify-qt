#pragma once

#include "lib/cache.hpp"
#include "lib/set.hpp"
#include "spotify/current.hpp"
#include "menu/track.hpp"
#include "enum/column.hpp"
#include "util/tooltip.hpp"

#include <QListWidget>
#include <QTreeWidgetItem>

namespace List
{
	class Tracks: public QTreeWidget
	{
	Q_OBJECT

	public:
		Tracks(lib::spt::api &spotify, lib::settings &settings, lib::cache &cache,
			const lib::http_client &httpClient, QWidget *parent);

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
		/**
		 * Offset index for header items
		 */
		static constexpr int titleOffset = 100;

		lib::settings &settings;
		lib::cache &cache;
		lib::spt::api &spotify;
		Tooltip tooltip;

		std::unordered_map<std::string, QTreeWidgetItem *> trackItems;

		QTreeWidgetItem *playingTrackItem = nullptr;
		QIcon emptyIcon;

		auto getCurrent() -> const spt::Current &;
		auto getAddedText(const std::string &date) const -> QString;
		auto getSelectedTrackIds() const -> std::vector<std::string>;
		void resizeHeaders(const QSize &newSize);

		void getLikedTracks(const std::function<void(const std::vector<lib::spt::track> &)> &callback);
		void updateLikedTracks(const std::function<void(const std::vector<lib::spt::track> &)> &callback);

		void onMenu(const QPoint &pos);
		void onItemClicked(QTreeWidgetItem *item, int column);
		void onDoubleClicked(QTreeWidgetItem *item, int column);
		void onHeaderMenu(const QPoint &pos);
		void onHeaderMenuTriggered(QAction *action);

		static void onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

		void onPlaybackRefreshed(const lib::spt::playback &current,
			const lib::spt::playback &previous);

		void onNewPlaylist();
		void onDelete();
		void onPlaySelectedRow();
		void onItemEntered(QTreeWidgetItem *item, int column);
	};
}
