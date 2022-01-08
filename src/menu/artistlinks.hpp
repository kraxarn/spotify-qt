#pragma once

#include "lib/httpclient.hpp"
#include "lib/spotify/artist.hpp"
#include "lib/search/api.hpp"

#include "util/icon.hpp"
#include "util/url.hpp"

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

		void addLink(const std::string &title, const std::string &url);

		void onAboutToShow();
		void onDuckDuckGo(bool checked);
		void onLoaded(const lib::ddg::results &results);
	};
}
