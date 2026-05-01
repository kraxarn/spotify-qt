#pragma once

#include "lib/cache.hpp"
#include "lib/httpclient.hpp"
#include "lib/lyrics/api.hpp"
#include "lib/spotify/playback.hpp"
#include "lib/spotify/track.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>

namespace View
{
	class Lyrics : public QWidget
	{
		Q_OBJECT

	public:
		Lyrics(const HttpClient &httpClient, lib::cache &cache, QWidget *parent);

		void open(const lib::spt::track &track);

		void open(unsigned int lyricsId);

		void setAutoUpdate(bool enabled);
		auto getCurrentTrack() const -> lib::spt::track;

		void clear();

	private:
		static constexpr int timestampRole = 0x100;
		bool autoUpdate = false;
		static constexpr float creditsFontScale = 0.9F;

		lib::cache &cache;
		LyricsApi lyrics;
		lib::spt::track currentTrack;
		unsigned int currentLyricsId = 0;

		QLabel *status;
		QListWidget *lyricsList;
		QListWidgetItem *currentLyricsItem;
		QCheckBox *syncWithMusic;
		QLabel *providedBy;

		void load(const ::Lyrics &loaded);

		static auto getTimestamp(const QListWidgetItem *item) -> qlonglong;

		static void setBold(QListWidgetItem *item, bool enabled);

		void onPlaybackRefreshed(const lib::spt::playback &playback,
			const lib::spt::playback &previous);

		void onMenuRequested(const QPoint &pos);

		void onCopy(bool checked) const;
	};
}
