#include "view/search/tracks.hpp"
#include "mainwindow.hpp"

Search::Tracks::Tracks(lib::spt::api &spotify, lib::cache &cache, QWidget *parent)
	: Search::SearchTabTree({"Title", "Artist", "Album"}, parent),
	spotify(spotify),
	cache(cache)
{
	// Hide "Album" by default
	header()->setSectionHidden(header()->count() - 1, true);

	QTreeWidget::connect(this, &QTreeWidget::itemActivated,
		this, &Search::Tracks::onItemActivated);

	setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(this, &QTreeWidget::customContextMenuRequested,
		this, &Search::Tracks::onContextMenu);
}

void Search::Tracks::resizeEvent(QResizeEvent *event)
{
	SearchTabTree::resizeEvent(event);

	header()->setSectionHidden(header()->count() - 1,
		event->size().width() < albumWidthThreshold);
}

void Search::Tracks::add(const lib::spt::track &track)
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

void Search::Tracks::onItemActivated(QTreeWidgetItem *item, int /*column*/)
{
	const auto &selectedItem = item->data(0, static_cast<int>(DataRole::Track))
		.value<lib::spt::track>();
	auto selectedIndex = -1;

	const auto itemCount = topLevelItemCount();
	std::vector<std::string> trackUris;
	trackUris.reserve(itemCount);

	for (auto i = 0; i < itemCount; i++)
	{
		const auto *current = topLevelItem(i);
		const auto &track = current->data(0, static_cast<int>(DataRole::Track))
			.value<lib::spt::track>();

		if (!track.is_valid())
		{
			continue;
		}

		if (selectedIndex < 0 && track.id == selectedItem.id)
		{
			selectedIndex = i;
		}

		trackUris.push_back(lib::spt::api::to_uri("track", track.id));
	}

	// Track wasn't found in list somehow, only play found track
	if (selectedIndex < 0)
	{
		selectedIndex = 0;
		trackUris.clear();
		trackUris.push_back(lib::spt::api::to_uri("track", selectedItem.id));
	}

	spotify.play_tracks(selectedIndex, trackUris, [](const std::string &status)
	{
		if (status.empty())
		{
			return;
		}

		StatusMessage::error(QString("Failed to play track: %1")
			.arg(QString::fromStdString(status)));
	});
}

void Search::Tracks::onContextMenu(const QPoint &pos)
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
