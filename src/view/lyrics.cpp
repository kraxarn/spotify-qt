#include "lyrics.hpp"
#include "mainwindow.hpp"

#include <QListWidgetItem>
#include <QVBoxLayout>

View::Lyrics::Lyrics(const lib::http_client &httpClient,
	lib::cache &cache, QWidget *parent)
	: QWidget(parent),
	cache(cache),
	lyrics(httpClient)
{
	lyrics.setAppInfo(QStringLiteral(APP_NAME), QStringLiteral(APP_VERSION),
		QStringLiteral("https://github.com/%1/%2")
		.arg(QStringLiteral(ORG_NAME), QStringLiteral(APP_NAME)));

	auto *layout = new QVBoxLayout(this);

	status = new QLabel(this);
	status->setAlignment(Qt::AlignHCenter);
	status->setWordWrap(true);
	layout->addWidget(status);

	lyricsList = new QListWidget(this);
	lyricsList->setWordWrap(true);
	lyricsList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
	layout->addWidget(lyricsList, 1);

	syncWithMusic = new QCheckBox(this);
	syncWithMusic->setContentsMargins(lyricsList->contentsMargins());
	syncWithMusic->setText(QStringLiteral("Sync with music"));
	syncWithMusic->setChecked(false);
	syncWithMusic->setVisible(false);
	layout->addWidget(syncWithMusic);

	providedBy = new QLabel(this);
	providedBy->setTextFormat(Qt::RichText);
	providedBy->setText(QStringLiteral("<i>Lyrics provided by <a href='https://lrclib.net'>lrclib</a></i>"));
	providedBy->setOpenExternalLinks(true);
	layout->addWidget(providedBy);
}

void View::Lyrics::open(const lib::spt::track &track)
{
	status->setText(QStringLiteral("Please wait..."));

	lyrics.get(track, [this, track](const Result<lib::lrc::lyrics> &result)
	{
		if (!result.success())
		{
			status->setText(result.message());
			return;
		}

		status->setVisible(false);
		load(result.value());
		currentTrack = track;
	});
}

void View::Lyrics::open(const unsigned int lyricsId)
{
	status->setText(QStringLiteral("Please wait..."));

	lyrics.get(lyricsId, [this](const Result<lib::lrc::lyrics> &result)
	{
		if (!result.success())
		{
			status->setText(result.message());
			return;
		}

		status->setVisible(false);
		load(result.value());
		currentTrack = {};
	});
}

void View::Lyrics::load(const lib::lrc::lyrics &loaded)
{
	lyricsList->clear();

	if (loaded.instrumental)
	{
		auto *item = new QListWidgetItem(lyricsList);
		item->setText(QStringLiteral("♪"));
	}
	else if (!loaded.synced_lyrics.empty())
	{
		for (const auto &line: loaded.synced_lyrics)
		{
			auto *item = new QListWidgetItem(lyricsList);
			item->setText(QString::fromStdString(line.text));
			item->setData(timestampRole, static_cast<qlonglong>(line.timestamp));

			if (lib::developer_mode::enabled)
			{
				item->setToolTip(QString::fromStdString(line.data));
			}
		}

		syncWithMusic->setChecked(true);
		syncWithMusic->setVisible(true);
	}
	else if (!loaded.plain_lyrics.empty())
	{
		for (const auto &line: loaded.plain_lyrics)
		{
			auto *item = new QListWidgetItem(lyricsList);
			item->setText(QString::fromStdString(line));
		}

		syncWithMusic->setChecked(false);
		syncWithMusic->setVisible(false);
	}
	else
	{
		return;
	}

	const auto *window = MainWindow::find(parentWidget());
	if (window == nullptr)
	{
		return;
	}

	connect(window, &MainWindow::playbackRefreshed,
		this, &View::Lyrics::onPlaybackRefreshed);
}

auto View::Lyrics::getTimestamp(const QListWidgetItem *item) -> qlonglong
{
	return item->data(timestampRole).toLongLong();
}

void View::Lyrics::onPlaybackRefreshed(const lib::spt::playback &playback,
	const lib::spt::playback &/*previous*/)
{
	if (!playback.is_playing || lyricsList->count() <= 0)
	{
		return;
	}

	if (playback.item.id != currentTrack.id || !syncWithMusic->isChecked())
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
		while (next != nullptr && (next->flags() & Qt::ItemIsEnabled) > 0)
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
		while (previous != nullptr && (previous->flags() & Qt::ItemIsEnabled) > 0)
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
