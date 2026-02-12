#pragma once

#include "lib/spotify/artist.hpp"
#include "lib/spotify/api.hpp"

#include "util/icon.hpp"
#include "menu/artistlinks.hpp"
#include "view/artist/sharemenu.hpp"

#include <QToolButton>
#include <QMenu>

namespace Artist
{
	class PlayButton: public QToolButton
	{
	public:
		PlayButton(lib::spt::api &spotify,
			const HttpClient &httpClient, QWidget *parent);

		void updateFollow(bool isFollowing) const;
		void setArtist(const lib::spt::artist &artist);

	private:
		lib::spt::api &spotify;
		const HttpClient &httpClient;
		lib::spt::artist artist;

		QAction *popularity = nullptr;
		QAction *follow = nullptr;

		auto contextMenu() -> QMenu *;

		void onClicked(bool checked);
		void onFollow(bool checked);
	};
}
