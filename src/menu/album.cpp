#include "menu/album.hpp"

#include "mainwindow.hpp"
#include "menu/addtoplaylist.hpp"

Menu::Album::Album(lib::spt::api &spotify, lib::cache &cache,
	const std::string &albumId, QWidget *parent)
	: QMenu(parent),
	spotify(spotify),
	cache(cache)
{
	album.id = albumId;

	trackCount = addAction("...");
	trackCount->setEnabled(false);
	addSeparator();

	auto *playShuffle = addAction(Icon::get("media-playlist-shuffle"), "Shuffle play");

	QAction::connect(playShuffle, &QAction::triggered,
		this, &Menu::Album::onShuffle);

	auto *share = addMenu(Icon::get("document-share"), "Share");

	auto *copyLink = share->addAction("Copy album link");
	QAction::connect(copyLink, &QAction::triggered,
		this, &Menu::Album::onCopyLink);

	auto *copyName = share->addAction(QStringLiteral("Copy album name"));
	QAction::connect(copyName, &QAction::triggered,
		this, &Menu::Album::onCopyName);

	auto *shareSongOpen = share->addAction("Open in Spotify");

	QAction::connect(shareSongOpen, &QAction::triggered,
		this, &Menu::Album::onOpenInSpotify);

	album = cache.get_album(albumId);
	if (album.is_valid())
	{
		tracksLoaded(cache.get_tracks(albumId));
	}
	else
	{
		spotify.album(albumId, [this](const lib::spt::album &item)
		{
			album = item;
			this->spotify.album_tracks(album, [this](const std::vector<lib::spt::track> &items)
			{
				this->tracksLoaded(items);
			});
		});
	}
}

void Menu::Album::onShuffle(bool /*checked*/)
{
	if (tracks.empty())
	{
		StatusMessage::warn(QStringLiteral("No tracks found to shuffle"));
		return;
	}

	const auto initialIndex = lib::random()
		.next_int(0, static_cast<int>(tracks.size()));

	spotify.play_tracks(initialIndex, lib::spt::id_to_uri("album", album.id),
		[this](const std::string &status)
		{
			if (!status.empty())
			{
				StatusMessage::error(QString::fromStdString(status));
				return;
			}

			spotify.set_shuffle(true, [](const std::string &status)
			{
				StatusMessage::error(QString::fromStdString(status));
			});
		});
}

void Menu::Album::onCopyLink(bool /*checked*/)
{
	QApplication::clipboard()->setText(QString("https://open.spotify.com/album/%1")
		.arg(QString::fromStdString(album.id)));
	StatusMessage::info(QStringLiteral("Link copied to clipboard"));
}

void Menu::Album::onCopyName(bool /*checked*/)
{
	QApplication::clipboard()->setText(QString::fromStdString(album.name));
	StatusMessage::info(QStringLiteral("Name copied to clipboard"));
}

void Menu::Album::onOpenInSpotify(bool /*checked*/)
{
	Url::open(QString("https://open.spotify.com/album/%1")
		.arg(QString::fromStdString(album.id)), LinkType::Web,
		MainWindow::find(parentWidget()));
}

void Menu::Album::tracksLoaded(const std::vector<lib::spt::track> &items)
{
	tracks = items;

	if (addToPlaylist == nullptr)
	{
		addToPlaylist = new Menu::AddToPlaylist(getTrackIds(), spotify, cache, this);
		addMenu(addToPlaylist);
	}

	auto duration = 0U;
	for (auto &track: tracks)
	{
		duration += track.duration;
	}

	constexpr unsigned int secInMin = 60U;
	constexpr unsigned int msToMin = 1000U * secInMin;
	auto minutes = duration / msToMin;

	trackCount->setText(QString("%1 %2, %3%4 m")
		.arg(tracks.size())
		.arg(tracks.size() == 1
			? QStringLiteral("track")
			: QStringLiteral("tracks"))
		.arg(minutes >= secInMin
			? QString("%1 h ").arg(minutes / secInMin)
			: QString())
		.arg(minutes % secInMin));
}

auto Menu::Album::getTrackIds() const -> std::vector<std::string>
{
	std::vector<std::string> trackIds;
	trackIds.reserve(tracks.size());

	for (const auto &track: tracks)
	{
		trackIds.push_back(track.id);
	}

	return trackIds;
}
