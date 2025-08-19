#pragma once

#include "lib/cache.hpp"
#include "lib/httpclient.hpp"
#include "lib/lyrics/api.hpp"
#include "lib/spotify/playback.hpp"
#include "lib/spotify/track.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
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
		void open(unsigned int lyricsId);

	private:
		static constexpr int timestampRole = 0x100;
		static constexpr float creditsFontScale = 0.9F;

		lib::cache &cache;
		lib::lrc::api lyrics;
		lib::spt::track currentTrack;

		QLabel *status;
		QListWidget *lyricsList;
		QCheckBox *syncWithMusic;
		QLabel *providedBy;

		void load(const lib::lrc::lyrics &loaded);
		static auto getTimestamp(const QListWidgetItem *item) -> qlonglong;

		void onPlaybackRefreshed(const lib::spt::playback &playback,
			const lib::spt::playback &previous);
	};
}
