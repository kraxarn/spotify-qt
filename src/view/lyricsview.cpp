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

	lyrics.get(track, [this](const std::vector<lib::lyrics_part> &parts)
	{
		if (parts.empty())
		{
			setPlainText("No results");
			return;
		}

		QString result;
		for (const auto &part: parts)
		{
			result.append(QString::fromStdString(part.second));
			result.append('\n');
		}

		setPlainText(result);
//		this->cache.set_track_info(track, info);
	});
}
