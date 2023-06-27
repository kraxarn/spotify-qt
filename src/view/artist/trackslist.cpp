#include "view/artist/trackslist.hpp"
#include "mainwindow.hpp"

Artist::TracksList::TracksList(lib::spt::api &spotify, lib::cache &cache, const lib::http_client &httpClient,
	const lib::spt::artist &artist, lib::settings &settings, QWidget *parent)
	: QListWidget(parent),
	spotify(spotify),
	cache(cache),
	httpClient(httpClient),
	artist(artist),
	tooltip(settings, httpClient, cache)
{
	setEnabled(false);
	setMouseTracking(true);

	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	QListWidget::connect(this, &QListWidget::itemDoubleClicked,
		this, &Artist::TracksList::onDoubleClicked);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &Artist::TracksList::onContextMenu);

	QListWidget::connect(this, &QListWidget::itemEntered,
		this, &Artist::TracksList::onItemEntered);
}

void Artist::TracksList::addTrack(const lib::spt::track &track)
{
	auto *item = new QListWidgetItem(QString::fromStdString(track.name), this);
	item->setData(static_cast<int>(DataRole::Track), QVariant::fromValue(track));

	Http::getAlbumImage(track.image_small(), httpClient, cache, [item, track](const QPixmap &image)
	{
		if (item != nullptr)
		{
			item->setIcon(QIcon(image));
		}
	});
}

void Artist::TracksList::onDoubleClicked(QListWidgetItem *currentItem)
{
	auto index = 0;
	std::vector<std::string> uris;

	const auto currentTrackData = currentItem->data(static_cast<int>(DataRole::Track));
	const auto currentTrackId = currentTrackData.value<lib::spt::track>().id;

	for (auto i = 0; i < count(); i++)
	{
		const auto trackData = item(i)->data(static_cast<int>(DataRole::Track));
		const auto trackId = trackData.value<lib::spt::track>().id;

		if (trackId == currentTrackId)
		{
			index = i;
		}

		uris.push_back(lib::spt::id_to_uri("track", trackId));
	}

	spotify.play_tracks(index, uris, [](const std::string &result)
	{
		if (result.empty())
		{
			return;
		}

		StatusMessage::error(QString("Failed to start playback: %1")
			.arg(QString::fromStdString(result)));
	});
}

void Artist::TracksList::onContextMenu(const QPoint &pos)
{
	const auto &items = selectedItems();

	QList<PlaylistTrack> tracks;
	tracks.reserve(items.size());

	for (const auto *item: items)
	{
		const auto &trackData = item->data(static_cast<int>(DataRole::Track));
		const auto &track = trackData.value<lib::spt::track>();

		if (!track.is_valid())
		{
			return;
		}

		tracks.append(PlaylistTrack(-1, track));
	}

	auto *songMenu = new Menu::Track(tracks, spotify, cache, &artist, parentWidget());
	songMenu->popup(mapToGlobal(pos));
}

void Artist::TracksList::onItemEntered(QListWidgetItem *item)
{
	if (!item->toolTip().isEmpty())
	{
		return;
	}

	const auto &trackData = item->data(static_cast<int>(DataRole::Track));
	const auto &track = trackData.value<lib::spt::track>();
	tooltip.set(item, track, item->icon());
}
