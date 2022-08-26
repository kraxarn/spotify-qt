#pragma once

#include "lib/httpclient.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/playback.hpp"
#include "lib/lyrics/api.hpp"
#include "lib/cache.hpp"

#include <QLabel>
#include <QListWidget>
#include <QComboBox>
#include <QCheckBox>
#include <QGridLayout>

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
		QCheckBox *syncWithMusic;

		QComboBox *lyricIds = nullptr;

		void load(int lyricsId);
		void load(const lib::lrc::lyrics &loaded);
		static auto getTimestamp(const QListWidgetItem *item) -> qlonglong;
		void setLyricsIds(const std::vector<lib::lrc::search_result> &results, int index);

		void onTick(const lib::spt::playback &playback);
		void onLyricsIdSelect(int index);
	};
}
