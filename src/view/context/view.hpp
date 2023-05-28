#pragma once

#include "view/context/title.hpp"
#include "view/context/expandedcontent.hpp"
#include "view/context/abstractcontent.hpp"
#include "view/context/smallcontent.hpp"
#include "lib/settings.hpp"
#include "lib/enum/albumsize.hpp"

#include <QDockWidget>

namespace Context
{
	class View: public QDockWidget
	{
	Q_OBJECT

	public:
		View(lib::spt::api &spotify, lib::settings &settings, lib::cache &cache,
			const lib::http_client &httpClient, QWidget *parent);

		void setAlbumSize(lib::album_size albumSize);
		void reset();

	private:
		lib::spt::api &spotify;
		lib::settings &settings;
		lib::cache &cache;
		const lib::http_client &httpClient;

		Title *title = nullptr;
		AbstractContent *albumContent = nullptr;

		void setAlbumImage(const lib::spt::entity &albumEntity, const std::string &albumImageUrl);

		void onPlaybackRefreshed(const lib::spt::playback &current,
			const lib::spt::playback &previous);
	};
}
