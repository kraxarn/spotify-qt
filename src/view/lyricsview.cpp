#include "lyricsview.hpp"

LyricsView::LyricsView(const lib::http_client &httpClient,
	lib::cache &cache, QWidget *parent)
	: http(httpClient),
	cache(cache),
	QDockWidget(parent)
{
	setWindowTitle("Lyrics");
	lyricsView = new QTextEdit(this);
	lyricsView->setReadOnly(true);
	setWidget(lyricsView);
	setMinimumWidth(300);
}

void LyricsView::open(const lib::spt::track &track)
{
	if (lyricsView == nullptr)
	{
		return;
	}

	setWindowTitle("...");
	lyricsView->setPlainText("Searching...");

	lib::lyrics lyrics(http);
	lyrics.get(track, [this, &track](bool success, const std::string &lyrics)
	{
		this->setWindowTitle(QString::fromStdString(track.title()));

		if (!success)
		{
			this->lyricsView->setPlainText(QString::fromStdString(lyrics));
			return;
		}

		this->lyricsView->setHtml(QString::fromStdString(lyrics).trimmed());
		this->cache.set_lyrics(track, lyrics);
	});
}
