#include "trackslist.hpp"
#include "mainwindow.hpp"

View::Artist::TracksList::TracksList(lib::spt::api &spotify, lib::cache &cache,
	const lib::http_client &httpClient, const lib::spt::artist &artist, QWidget *parent)
	: spotify(spotify),
	cache(cache),
	httpClient(httpClient),
	artist(artist),
	QListWidget(parent)
{
	setEnabled(false);

	QListWidget::connect(this, &QListWidget::itemActivated,
		this, &View::Artist::TracksList::onActivated);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &View::Artist::TracksList::onContextMenu);
}

void View::Artist::TracksList::addTrack(const lib::spt::track &track)
{
	auto *item = new QListWidgetItem(QString::fromStdString(track.name), this);
	item->setData(RoleTrack, QVariant::fromValue(track));

	HttpUtils::getAlbum(track.image, httpClient, cache, [item](const QPixmap &image)
	{
		if (item != nullptr)
		{
			item->setIcon(QIcon(image));
		}
	});
}

void View::Artist::TracksList::onActivated(QListWidgetItem *currentItem)
{
	auto index = 0;
	std::vector<std::string> uris;
	auto currentTrackId = currentItem->data(RoleTrack).value<lib::spt::track>().id;

	for (auto i = 0; i < count(); i++)
	{
		const auto trackId = this->item(i)->data(RoleTrack).value<lib::spt::track>().id;
		if (trackId == currentTrackId)
		{
			index = i;
		}
		uris.push_back(lib::spt::api::to_uri("track", trackId));
	}

	spotify.play_tracks(index, uris, [this](const std::string &result)
	{
		if (result.empty())
		{
			return;
		}

		auto *mainWindow = MainWindow::find(this->parentWidget());
		mainWindow->status(lib::fmt::format("Failed to start playback: {}",
			result), true);
	});
}

void View::Artist::TracksList::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	const auto &track = item->data(RoleTrack).value<lib::spt::track>();
	if (!track.is_valid())
	{
		return;
	}

	auto *songMenu = new SongMenu(track, spotify, cache, &artist, parentWidget());
	songMenu->popup(mapToGlobal(pos));
}
