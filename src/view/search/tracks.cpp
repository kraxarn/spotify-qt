#include "view/search/tracks.hpp"
#include "mainwindow.hpp"

View::Search::Tracks::Tracks(lib::spt::api &spotify, lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	cache(cache),
	View::Search::SearchTabTree({"Title", "Artist", "Album"}, parent)
{
	// Hide "Album" by default
	header()->setSectionHidden(header()->count() - 1, true);

	QTreeWidget::connect(this, &QTreeWidget::itemActivated,
		this, &View::Search::Tracks::onItemActivated);

	setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(this, &QTreeWidget::customContextMenuRequested,
		this, &View::Search::Tracks::onContextMenu);
}

void View::Search::Tracks::resizeEvent(QResizeEvent *event)
{
	SearchTabTree::resizeEvent(event);

	header()->setSectionHidden(header()->count() - 1,
		event->size().width() < albumWidthThreshold);
}

void View::Search::Tracks::add(const lib::spt::track &track)
{
	auto trackName = QString::fromStdString(track.name);
	auto trackArtist = QString::fromStdString(lib::spt::entity::combine_names(track.artists));
	auto trackAlbum = QString::fromStdString(track.album.name);

	auto *item = new QTreeWidgetItem(this, {
		trackName,
		trackArtist,
		trackAlbum,
	});

	item->setData(0, static_cast<int>(DataRole::Track),
		QVariant::fromValue(track));
	item->setToolTip(0, trackName);
	item->setToolTip(1, trackArtist);
}

void View::Search::Tracks::onItemActivated(QTreeWidgetItem *item, int /*column*/)
{
	// Do we want it to continue playing results?
	const auto &track = item->data(0, static_cast<int>(DataRole::Track))
		.value<lib::spt::track>();
	auto trackId = lib::spt::api::to_uri("track", track.id);

	spotify.play_tracks(0, {trackId}, [](const std::string &status)
	{
		if (status.empty())
		{
			return;
		}

		StatusMessage::error(QString("Failed to play track: %1")
			.arg(QString::fromStdString(status)));
	});
}

void View::Search::Tracks::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	const auto &track = item->data(0, static_cast<int>(DataRole::Track))
		.value<lib::spt::track>();
	if (!track.is_valid())
	{
		return;
	}

	auto *menu = new SongMenu(track, spotify, cache, parentWidget());
	menu->popup(mapToGlobal(pos));
}
