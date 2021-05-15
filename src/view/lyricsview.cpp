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
	const auto &cached = cache.get_track_info(track);
	if (cached.is_valid())
	{
		setPlainText(QString::fromStdString(cached.lyrics));
		return;
	}

	setPlainText("Searching...");

	lyrics.get(track, [this, &track](bool success, const std::string &result)
	{
		try
		{
			if (!success)
			{
				this->setPlainText(QString::fromStdString(result));
				return;
			}

			lib::spt::track_info info = nlohmann::json::parse(result);

			this->setPlainText(QString::fromStdString(info.lyrics));
			this->cache.set_track_info(track, info);
		}
		catch (const std::exception &e)
		{
			this->setPlainText(e.what());
		}
	});
}
