#pragma once

#include "lib/httpclient.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/playback.hpp"
#include "lib/lyrics/api.hpp"
#include "lib/cache.hpp"

#include <QLabel>
#include <QListWidget>

namespace View
{
	class Lyrics: public QWidget
	{
	Q_OBJECT

	public:
		Lyrics(const lib::http_client &httpClient,
			lib::cache &cache, QWidget *parent);

		void open(const lib::spt::track &track);

	private:
		static constexpr int timestampRole = 0x100;

		lib::cache &cache;
		lib::lrc::api lyrics;
		lib::spt::track currentTrack;

		QLabel *status;
		QListWidget *lyricsList;

		void load(const lib::lrc::lyrics &loaded);
		static auto getTimestamp(const QListWidgetItem *item) -> qlonglong;

		void onTick(const lib::spt::playback &playback);
	};
}
