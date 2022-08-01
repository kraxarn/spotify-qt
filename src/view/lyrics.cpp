#include "lyrics.hpp"

#include <QVBoxLayout>

View::Lyrics::Lyrics(const lib::http_client &httpClient,
	lib::cache &cache, QWidget *parent)
	: QWidget(parent),
	cache(cache),
	lyrics(httpClient)
{
	auto *layout = new QVBoxLayout(this);

	status = new QLabel(this);
	layout->addWidget(status);

	lyricsList = new QListWidget(this);
	lyricsList->setWordWrap(true);
	layout->addWidget(lyricsList);
}

void View::Lyrics::open(const lib::spt::track &track)
{
//	const auto &cached = cache.get_track_info(track);
//	if (cached.is_valid())
//	{
//		setPlainText(QString::fromStdString(cached.lyrics));
//		return;
//	}

	status->setText(QStringLiteral("Searching..."));

	lyrics.get(track, [this](const std::vector<lib::lyrics_part> &parts)
	{
		if (parts.empty())
		{
			status->setText(QStringLiteral("No results"));
			return;
		}

		status->setVisible(false);

		for (const auto &part: parts)
		{
			auto *item = new QListWidgetItem(lyricsList);
			item->setText(QString::fromStdString(part.second));
			item->setData(timestampRole, (qlonglong) part.first);
		}

//		this->cache.set_track_info(track, info);
	});
}
