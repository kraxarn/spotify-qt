#include "view/search/playlists.hpp"
#include "mainwindow.hpp"

Search::Playlists::Playlists(lib::spt::api &spotify, lib::cache &cache, const lib::http_client &httpClient,
	lib::settings &settings, QWidget *parent)
	: QListWidget(parent),
	spotify(spotify),
	cache(cache),
	tooltip(settings, httpClient, cache)
{
	setMouseTracking(true);

	QListWidget::connect(this, &QListWidget::itemClicked,
		this, &Search::Playlists::onItemClicked);

	QListWidget::connect(this, &QListWidget::itemDoubleClicked,
		this, &Search::Playlists::onItemDoubleClicked);

	setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &Search::Playlists::onContextMenu);

	QListWidget::connect(this, &QListWidget::itemEntered,
		this, &Search::Playlists::onItemEntered);
}

void Search::Playlists::add(const lib::spt::playlist &playlist)
{
	auto playlistName = QString::fromStdString(playlist.name);
	auto playlistId = QString::fromStdString(playlist.id);

	auto *item = new QListWidgetItem(playlistName, this);
	item->setData(static_cast<int>(DataRole::Playlist), QVariant::fromValue(playlist));
}

void Search::Playlists::onItemClicked(QListWidgetItem *item)
{
	const auto &playlistData = item->data(static_cast<int>(DataRole::Playlist));
	const auto playlist = playlistData.value<lib::spt::playlist>();

	spotify.playlist(playlist.id, [this](const lib::result<lib::spt::playlist> &result)
	{
		if (!result.success())
		{
			StatusMessage::error(QStringLiteral("Failed to load playlist: %1")
				.arg(QString::fromStdString(result.message())));

			return;
		}

		auto *mainWindow = MainWindow::find(this->parentWidget());
		mainWindow->getSongsTree()->load(result.value());
		mainWindow->setCurrentPlaylistItem(-1);
	});
}

void Search::Playlists::onItemDoubleClicked(QListWidgetItem *item)
{
	const auto &playlistData = item->data(static_cast<int>(DataRole::Playlist));
	const auto playlist = playlistData.value<lib::spt::playlist>();

	spotify.play_tracks(lib::spt::id_to_uri("playlist", playlist.id), [](const std::string &result)
	{
		if (!result.empty())
		{
			StatusMessage::error(QStringLiteral("Failed to start playlist playback: %1")
				.arg(QString::fromStdString(result)));
		}
	});
}

void Search::Playlists::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	const auto &playlistData = item->data(static_cast<int>(DataRole::Playlist));
	const auto playlist = playlistData.value<lib::spt::playlist>();

	auto *menu = new Menu::Playlist(spotify, playlist, cache, parentWidget());
	menu->popup(mapToGlobal(pos));
}

void Search::Playlists::onItemEntered(QListWidgetItem *item)
{
	if (!item->toolTip().isEmpty())
	{
		return;
	}

	const auto &playlistData = item->data(static_cast<int>(DataRole::Playlist));
	const auto playlist = playlistData.value<lib::spt::playlist>();
	tooltip.set(item, playlist);
}
