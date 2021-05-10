#include "lyricsview.hpp"

LyricsView::LyricsView(const lib::http_client &httpClient,
	lib::cache &cache, QWidget *parent)
	: cache(cache),
	lyrics(httpClient),
	QTextEdit(parent)
{
	setReadOnly(true);
}

void LyricsView::open(const lib::spt::track &track)
{
	const auto &cached = cache.get_lyrics(track);
	if (!cached.empty())
	{
		setHtml(QString::fromStdString(cached));
		return;
	}

	setPlainText("Searching...");

	lyrics.get(track, [this, &track](bool success, const std::string &result)
	{
		if (!success)
		{
			this->setPlainText(QString::fromStdString(result));
			return;
		}

		this->setHtml(QString::fromStdString(result).trimmed());
		this->cache.set_lyrics(track, result);
	});
}
