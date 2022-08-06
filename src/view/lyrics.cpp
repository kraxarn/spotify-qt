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
	lyricsList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
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

	lyrics.get(track, [this, track](const std::vector<lib::lyrics_part> &parts)
	{
		if (parts.empty())
		{
			status->setText(QStringLiteral("No results"));
			return;
		}

		currentTrack = track;
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

	if (playback.item.id != currentTrack.id)
	{
		lyricsList->setCurrentItem(nullptr);
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

	if (getTimestamp(item) < playback.progress_ms)
	{
		QListWidgetItem *next = lyricsList->item(++index);
		while (next != nullptr)
		{
			const auto nextTimestamp = getTimestamp(next);
			if (nextTimestamp > playback.progress_ms)
			{
				break;
			}

			item = next;
			next = lyricsList->item(++index);
		}
	}
	else
	{
		QListWidgetItem *previous = lyricsList->item(--index);
		while (previous != nullptr)
		{
			const auto previousTimestamp = getTimestamp(previous);
			if (previousTimestamp < playback.progress_ms)
			{
				break;
			}

			item = previous;
			previous = lyricsList->item(--index);
		}
	}

	lyricsList->setCurrentItem(item);
	emit lyricsList->scrollToItem(item, QAbstractItemView::PositionAtCenter);
}
