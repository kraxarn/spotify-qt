#include "searchtab/tracks.hpp"
#include "mainwindow.hpp"

SearchTab::Tracks::Tracks(lib::spt::api &spotify, lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	cache(cache),
	SearchTab::SearchTabTree({"Title", "Artist", "Album"}, parent)
{
	QTreeWidget::connect(this, &QTreeWidget::itemActivated,
		this, &SearchTab::Tracks::onItemActivated);

	setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(this, &QTreeWidget::customContextMenuRequested,
		this, &SearchTab::Tracks::onContextMenu);
}

void SearchTab::Tracks::add(const lib::spt::track &track)
{
	auto trackName = QString::fromStdString(track.name);
	auto trackArtist = QString::fromStdString(lib::spt::entity::combine_names(track.artists));
	auto *item = new QTreeWidgetItem(this, {
		trackName, trackArtist
	});
	item->setData(0, RoleTrack, QVariant::fromValue(track));
	item->setData(0, RoleAlbumId, QString::fromStdString(track.album.id));
	item->setToolTip(0, trackName);
	item->setToolTip(1, trackArtist);
}

void SearchTab::Tracks::onItemActivated(QTreeWidgetItem *item, int /*column*/)
{
	// Do we want it to continue playing results?
	const auto &track = item->data(0, RoleTrack).value<lib::spt::track>();
	auto trackId = lib::spt::api::to_uri("track", track.id);

	spotify.play_tracks(0, {trackId}, [this](const std::string &status)
	{
		if (status.empty())
		{
			return;
		}

		auto *mainWindow = MainWindow::find(this->parentWidget());
		mainWindow->status(lib::fmt::format("Failed to play track: {}",
			status), true);
	});
}

void SearchTab::Tracks::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	const auto &track = item->data(0, RoleTrack).value<lib::spt::track>();
	if (!track.is_valid())
	{
		return;
	}

	auto *menu = new SongMenu(track, spotify, cache, parentWidget());
	menu->popup(mapToGlobal(pos));
}
