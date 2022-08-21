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

	if (lib::developer_mode::enabled)
	{
		lyricIds = new QComboBox(this);
		lyricIds->setMaximumWidth(250);
		lyricIds->setVisible(false);
		layout->addWidget(lyricIds, 0, Qt::AlignHCenter);
	}

	lyricsList = new QListWidget(this);
	lyricsList->setWordWrap(true);
	lyricsList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	layout->addWidget(lyricsList);

	syncWithMusic = new QCheckBox(this);
	syncWithMusic->setText(QStringLiteral("Sync with music"));
	syncWithMusic->setChecked(false);
	syncWithMusic->setVisible(false);
	layout->addWidget(syncWithMusic);
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

	lyrics.search(track,
		[this, track](const lib::result<std::vector<lib::lrc::search_result>> &result)
		{
			if (!result.success())
			{
				status->setText(QString::fromStdString(result.message()));
				return;
			}

			auto strip = [](const std::string &str) -> std::string
			{
				return lib::strings::to_lower(lib::strings::erase_non_alpha(str));
			};

			size_t index = -1;
			const auto albumName = strip(track.album.name);
			const auto &results = result.value();

			for (size_t i = 0; i < results.size(); i++)
			{
				if (strip(results[i].album) == albumName)
				{
					index = i;
					break;
				}
			}

			if (index >= results.size())
			{
				status->setText(QStringLiteral("No results"));
				return;
			}

			if (lyricIds != nullptr && results.size() > 1)
			{
				setLyricsIds(results, static_cast<int>(index));
			}

			load(results[index].lyrics_id);
			currentTrack = track;
		});
}

void View::Lyrics::load(int lyricsId)
{
	lyrics.lyrics(lyricsId, [this](const lib::result<lib::lrc::lyrics> &result)
	{
		if (!result.success())
		{
			status->setText(QString::fromStdString(result.message()));
			return;
		}

		status->setVisible(false);
		load(result.value());
	});
}

void View::Lyrics::load(const lib::lrc::lyrics &loaded)
{
	lyricsList->clear();
	if (loaded.lines.empty())
	{
		return;
	}

	for (const auto &line: loaded.lines)
	{
		auto *item = new QListWidgetItem(lyricsList);
		item->setText(QString::fromStdString(line.text));
		item->setData(timestampRole, (qlonglong) line.timestamp);

		if (lib::developer_mode::enabled)
		{
			item->setToolTip(QString::fromStdString(line.data));
		}
	}

	syncWithMusic->setChecked(true);
	syncWithMusic->setVisible(true);

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

void View::Lyrics::setLyricsIds(const std::vector<lib::lrc::search_result> &results, int index)
{
	lyricIds->clear();

	for (const auto &result: results)
	{
		lyricIds->addItem(QString("%1 - %2 - %3")
				.arg(QString::fromStdString(lib::strings::join(result.artists, ", ")))
				.arg(QString::fromStdString(result.track))
				.arg(QString::fromStdString(result.album)),
			result.lyrics_id);
	}

	lyricIds->setCurrentIndex(index);

	if (!lyricIds->isVisible())
	{
		lyricIds->show();
		QComboBox::connect(lyricIds, QOverload<int>::of(&QComboBox::currentIndexChanged),
			this, &View::Lyrics::onLyricsIdSelect);
	}
}

void View::Lyrics::onTick(const lib::spt::playback &playback)
{
	if (!playback.is_playing || !syncWithMusic->isChecked())
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

void View::Lyrics::onLyricsIdSelect(int index)
{
	const auto lyricsId = lyricIds->itemData(index);
	if (lyricsId.canConvert<int>())
	{
		load(lyricsId.toInt());
	}
}
