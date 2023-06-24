#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "enum/datarole.hpp"
#include "util/http.hpp"

#include <QListWidget>

namespace Artist
{
	class TracksList: public QListWidget
	{
	public:
		TracksList(lib::spt::api &spotify, lib::cache &cache, const lib::http_client &httpClient,
			const lib::spt::artist &artist, lib::settings &settings, QWidget *parent);

		void addTrack(const lib::spt::track &track);

	private:
		lib::spt::api &spotify;
		lib::cache &cache;
		const lib::http_client &httpClient;
		const lib::spt::artist &artist;
		lib::settings &settings;

		void onDoubleClicked(QListWidgetItem *currentItem);
		void onContextMenu(const QPoint &pos);
	};
}
