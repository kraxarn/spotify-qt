#pragma once

#include "lib/httpclient.hpp"
#include "lib/spotify/artist.hpp"
#include "lib/search/api.hpp"

#include "util/icon.hpp"
#include "util/urlutils.hpp"

#include <QMenu>

namespace Menu
{
	class ArtistLinks: public QMenu
	{
	public:
		ArtistLinks(const lib::spt::artist &artist,
			const lib::http_client &httpClient, QWidget *parent);

	private:
		const lib::spt::artist &artist;
		lib::ddg::api ddg;
		QAction *loading = nullptr;

		void onAboutToShow();
		void onDuckDuckGo(bool checked);
		void onLoaded(const lib::ddg::results &results);
	};
}
