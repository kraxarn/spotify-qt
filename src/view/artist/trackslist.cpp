#include "trackslist.hpp"
#include "mainwindow.hpp"

Artist::TracksList::TracksList(lib::spt::api &spotify, lib::cache &cache,
	const lib::http_client &httpClient, const lib::spt::artist &artist, QWidget *parent)
	: QListWidget(parent),
	spotify(spotify),
	cache(cache),
	httpClient(httpClient),
	artist(artist)
{
	setEnabled(false);

	QListWidget::connect(this, &QListWidget::itemActivated,
		this, &Artist::TracksList::onActivated);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &Artist::TracksList::onContextMenu);
}

void Artist::TracksList::addTrack(const lib::spt::track &track)
{
	auto *item = new QListWidgetItem(QString::fromStdString(track.name), this);
	item->setData(static_cast<int>(DataRole::Track), QVariant::fromValue(track));

	Http::getAlbum(track.image, httpClient, cache, [item](const QPixmap &image)
	{
		if (item != nullptr)
		{
			item->setIcon(QIcon(image));
		}
	});
}

void Artist::TracksList::onActivated(QListWidgetItem *currentItem)
{
	auto index = 0;
	std::vector<std::string> uris;
	auto currentTrackId = currentItem->data(static_cast<int>(DataRole::Track))
		.value<lib::spt::track>().id;

	for (auto i = 0; i < count(); i++)
	{
		const auto trackId = this->item(i)->data(static_cast<int>(DataRole::Track))
			.value<lib::spt::track>().id;
		if (trackId == currentTrackId)
		{
			index = i;
		}
		uris.push_back(lib::spt::api::to_uri("track", trackId));
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
	auto *item = itemAt(pos);
	const auto &track = item->data(static_cast<int>(DataRole::Track))
		.value<lib::spt::track>();
	if (!track.is_valid())
	{
		return;
	}

	auto *songMenu = new Menu::Track(track, spotify, cache, &artist, parentWidget());
	songMenu->popup(mapToGlobal(pos));
}
