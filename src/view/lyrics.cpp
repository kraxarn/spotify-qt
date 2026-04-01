#include "lyrics.hpp"
#include "mainwindow.hpp"

#include <QClipboard>
#include <QListWidgetItem>
#include <QVBoxLayout>

View::Lyrics::Lyrics(const HttpClient &httpClient,
	lib::cache &cache, QWidget *parent)
	: QWidget(parent),
	cache(cache),
	lyrics(httpClient),
	currentLyricsItem(nullptr)
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
	lyricsList->setSelectionMode(QAbstractItemView::ContiguousSelection);
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

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	connect(this, &QWidget::customContextMenuRequested,
		this, &Lyrics::onMenuRequested);

	const auto *window = MainWindow::find(parentWidget());
	if (window != nullptr)
	{
		connect(window, &MainWindow::playbackRefreshed,
			this, &View::Lyrics::onPlaybackRefreshed);
	}
}

void View::Lyrics::setAutoUpdate(bool enabled)
{
	autoUpdate = enabled;
}

auto View::Lyrics::getCurrentTrack() const -> lib::spt::track
{
	return currentTrack;
}

void View::Lyrics::clear()
{
	lyricsList->clear();
	currentTrack = {};
	currentLyricsId = 0;
	currentLyricsItem = nullptr;
	status->setText(QStringLiteral("Nothing playing..."));
	status->setVisible(true);
}

void View::Lyrics::open(const lib::spt::track &track)
{
	status->setText(QStringLiteral("Please wait..."));
	status->setVisible(true);

	currentTrack = track;
	currentLyricsId = 0;
	currentLyricsItem = nullptr;

	lyrics.get(track, [this, track](const Result<::Lyrics> &result)
	{
		if (currentTrack.id != track.id)
		{
			return;
		}

		if (!result.success())
		{
			status->setText(result.message());
			status->setVisible(true);
			return;
		}

		status->setVisible(false);
		load(result.value());
	});
}

void View::Lyrics::open(const unsigned int lyricsId)
{
	status->setText(QStringLiteral("Please wait..."));
	status->setVisible(true);

	currentTrack = {};
	currentLyricsId = lyricsId;
	currentLyricsItem = nullptr;

	lyrics.get(lyricsId, [this, lyricsId](const Result<::Lyrics> &result)
	{
		if (currentLyricsId != lyricsId)
		{
			return;
		}

		if (!result.success())
		{
			status->setText(result.message());
			status->setVisible(true);
			return;
		}

		status->setVisible(false);
		load(result.value());
	});
}

void View::Lyrics::load(const ::Lyrics &loaded)
{
	currentLyricsItem = nullptr;
	lyricsList->clear();

	if (loaded.instrumental())
	{
		auto *item = new QListWidgetItem(lyricsList);
		item->setText(QStringLiteral("♪"));
	}
	else if (!loaded.syncedLyrics().isEmpty())
	{
		for (const LyricsLine &line: loaded.syncedLyrics())
		{
			auto *item = new QListWidgetItem(lyricsList);
			item->setText(line.text());
			item->setData(timestampRole, static_cast<qlonglong>(line.timestamp()));

			if (lib::developer_mode::enabled)
			{
				item->setToolTip(line.data());
			}
		}

		syncWithMusic->setChecked(true);
		syncWithMusic->setVisible(true);
	}
	else if (!loaded.plainLyrics().isEmpty())
	{
		for (const auto &line: loaded.plainLyrics())
		{
			auto *item = new QListWidgetItem(lyricsList);
			item->setText(line);
		}

		syncWithMusic->setChecked(false);
		syncWithMusic->setVisible(false);
	}
	else
	{
		return;
	}
}

auto View::Lyrics::getTimestamp(const QListWidgetItem *item) -> qlonglong
{
	return item->data(timestampRole).toLongLong();
}

void View::Lyrics::setBold(QListWidgetItem *item, const bool enabled)
{
	if (item == nullptr)
	{
		return;
	}

	QFont font = item->font();
	font.setBold(enabled);
	item->setFont(font);
}

void View::Lyrics::onPlaybackRefreshed(const lib::spt::playback &playback,
	const lib::spt::playback &/*previous*/)
{
	if (autoUpdate && playback.item.id != currentTrack.id)
	{
		if (playback.item.is_valid())
		{
			open(playback.item);
		}
		else
		{
			clear();
		}
		return;
	}

	if (!playback.is_playing || lyricsList->count() <= 0)
	{
		return;
	}

	if (playback.item.id != currentTrack.id || !syncWithMusic->isChecked())
	{
		setBold(currentLyricsItem, false);
		currentLyricsItem = nullptr;
		return;
	}

	QListWidgetItem *item;
	int index;

	if (currentLyricsItem == nullptr)
	{
		index = 0;
		item = lyricsList->item(index);
	}
	else
	{
		index = lyricsList->row(currentLyricsItem);
		item = currentLyricsItem;
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

	setBold(currentLyricsItem, false);
	setBold(item, true);
	currentLyricsItem = item;

	emit lyricsList->scrollToItem(item, QAbstractItemView::PositionAtCenter);
}

void View::Lyrics::onMenuRequested(const QPoint &pos)
{
	const auto menu = new QMenu(this);

	const QAction *copy = menu->addAction(QStringLiteral("Copy selection"));
	connect(copy, &QAction::triggered, this, &Lyrics::onCopy);

	menu->popup(mapToGlobal(pos));
}

void View::Lyrics::onCopy([[maybe_unused]] bool checked) const
{
	QString text;

	for (const QListWidgetItem *item: lyricsList->selectedItems())
	{
		text.append(item->text());

		if (item != lyricsList->selectedItems().last())
		{
			text.append(QChar::LineSeparator);
		}
	}

	QApplication::clipboard()->setText(text);
}
