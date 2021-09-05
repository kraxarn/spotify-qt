#include "lyricsview.hpp"

LyricsView::LyricsView(const lib::http_client &httpClient,
	lib::cache &cache, QWidget *parent)
	: QTextEdit(parent),
	cache(cache),
	lyrics(httpClient)
{
	setReadOnly(true);
}

void LyricsView::open(const lib::spt::track &track)
{
	const auto &cached = cache.get_track_info(track);
	if (cached.is_valid())
	{
		setPlainText(QString::fromStdString(cached.lyrics));
		return;
	}

	setPlainText("Searching...");

	lyrics.get(track, [this, &track](const lib::spt::track_info &info)
	{
		if (!info.is_valid())
		{
			this->setPlainText("No results");
			return;
		}
		this->setPlainText(QString::fromStdString(info.lyrics));
		this->cache.set_track_info(track, info);
	});
}
