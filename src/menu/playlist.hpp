#pragma once

#include "dialog/playlistedit.hpp"
#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "lib/random.hpp"

#include <QInputDialog>
#include <QMenu>
#include <QUrl>

namespace Menu
{
	class Playlist: public QMenu
	{
	Q_OBJECT

	public:
		Playlist(lib::spt::api &spotify, const lib::spt::playlist &playlist,
			lib::cache &cache, QWidget *parent);

	protected:
		void showEvent(QShowEvent *event) override;

	private:
		lib::spt::playlist playlist;
		lib::cache &cache;
		lib::spt::api &spotify;

		Dialog::PlaylistEdit *editDialog = nullptr;
		std::vector<lib::spt::track> tracks;
		QAction *tracksAction = nullptr;
		QAction *byAction = nullptr;
		QAction *editAction = nullptr;
		QAction *followAction = nullptr;

		void tracksLoaded(const std::vector<lib::spt::track> &items);
		void isFollowingLoaded(const std::vector<bool> &follows);

		auto playlistUrl() const -> QString;

		auto shareMenu() -> QMenu *;
		auto devMenu() -> QMenu *;

		void onShuffle(bool checked);
		void onEdit(bool checked);
		void onRefresh(bool checked);
		void onFollow(bool checked);
		void onCopyLink(bool checked) const;
		void onOpenInSpotify(bool checked) const;
		void onCopyId(bool checked) const;
		void onShowJson(bool checked) const;
	};
}
