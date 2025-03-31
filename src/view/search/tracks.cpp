#include "view/search/tracks.hpp"

#include "mainwindow.hpp"
#include "metatypes.hpp"

Search::Tracks::Tracks(lib::spt::api &spotify, lib::cache &cache,
	lib::settings &settings, const lib::http_client &httpClient, QWidget *parent)
	: Search::SearchTabTree({"Title", "Artist", "Album"}, parent),
	spotify(spotify),
	cache(cache),
	tooltip(settings, httpClient, cache)
{
	// Hide "Album" by default
	header()->setSectionHidden(header()->count() - 1, true);

	setMouseTracking(true);

	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	QTreeWidget::connect(this, &QTreeWidget::itemDoubleClicked,
		this, &Search::Tracks::onItemDoubleClicked);

	setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(this, &QTreeWidget::customContextMenuRequested,
		this, &Search::Tracks::onContextMenu);

	QTreeWidget::connect(this, &QTreeWidget::itemEntered,
		this, &Search::Tracks::onItemEntered);
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
	item->setToolTip(1, trackArtist);
}

void Search::Tracks::onItemDoubleClicked(QTreeWidgetItem *item, int /*column*/)
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

		trackUris.push_back(lib::spt::id_to_uri("track", track.id));
	}

	// Track wasn't found in list somehow, only play found track
	if (selectedIndex < 0)
	{
		selectedIndex = 0;
		trackUris.clear();
		trackUris.push_back(lib::spt::id_to_uri("track", selectedItem.id));
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
	const auto &items = selectedItems();

	QList<PlaylistTrack> tracks;
	tracks.reserve(items.size());

	for (const auto *item: items)
	{
		const auto &trackData = item->data(0, static_cast<int>(DataRole::Track));
		const auto &track = trackData.value<lib::spt::track>();

		if (!track.is_valid())
		{
			return;
		}

		tracks.append(PlaylistTrack(-1, track));
	}

	auto *songMenu = new Menu::Track(tracks, spotify, cache, parentWidget());
	songMenu->popup(mapToGlobal(pos));
}

void Search::Tracks::onItemEntered(QTreeWidgetItem *item, int column)
{
	if (!item->toolTip(0).isEmpty() || column != 0)
	{
		return;
	}

	const auto trackData = item->data(0, static_cast<int>(DataRole::Track));
	const auto track = trackData.value<lib::spt::track>();
	tooltip.set(item, track);
}
