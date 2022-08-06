#include "lyrics.hpp"
#include "mainwindow.hpp"

#include <QVBoxLayout>
#include <QList>
#include <QListWidgetItem>

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
		load(parts);

//		this->cache.set_track_info(track, info);
	});
}

void View::Lyrics::load(const std::vector<lib::lyrics_part> &parts)
{
	lyricsList->clear();
	if (parts.empty())
	{
		return;
	}

	for (const auto &part: parts)
	{
		auto *item = new QListWidgetItem(lyricsList);
		item->setText(QString::fromStdString(part.second));
		item->setData(timestampRole, (qlonglong) part.first);
	}

	auto *window = MainWindow::find(parentWidget());
	if (window == nullptr)
	{
		return;
	}

	MainWindow::connect(window, &MainWindow::tick, this, &View::Lyrics::onTick);
}

auto View::Lyrics::getTimestamp(const QListWidgetItem *item) -> qlonglong
{
	return item->data(timestampRole).toLongLong();
}

void View::Lyrics::onTick(const lib::spt::playback &playback)
{
	if (!playback.is_playing)
	{
		return;
	}

	auto *currentItem = lyricsList->currentItem();
	QListWidgetItem *item;
	int index;

	if (currentItem == nullptr)
	{
		index = 0;
		item = lyricsList->item(index);
	}
	else
	{
		index = lyricsList->currentRow();
		item = currentItem;
	}

	auto timestamp = getTimestamp(item);

	if (timestamp < playback.progress_ms)
	{
		// We are behind, check next
		while (timestamp < playback.progress_ms)
		{
			item = lyricsList->item(++index);
			if (item == nullptr)
			{
				break;
			}
			timestamp = getTimestamp(item);
		}
	}
	else
	{
		// We are ahead, check previous
		while (timestamp > playback.progress_ms)
		{
			item = lyricsList->item(--index);
			if (item == nullptr)
			{
				break;
			}
			timestamp = getTimestamp(item);
		}
	}

	lyricsList->setCurrentItem(item);
	emit lyricsList->scrollToItem(item, QAbstractItemView::PositionAtCenter);

	lib::log::debug("Now: {}", playback.progress_ms);
}
