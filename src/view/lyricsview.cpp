#include "lyricsview.hpp"

LyricsView::LyricsView(lib::spt::api &spotify, lib::cache &cache, QWidget *parent)
	: QTextEdit(parent),
	spotify(spotify),
	cache(cache)
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

	spotify.lyrics(track, [this, &track](const std::string &response)
	{
		if (response.empty())
		{
			this->setPlainText("No results");
			return;
		}
		this->setPlainText(QString::fromStdString(response));
	});
}
