#include "view/context/title.hpp"

#include "mainwindow.hpp"

View::Context::Title::Title(lib::spt::api &spotify, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	current(current),
	cache(cache),
	QWidget(parent)
{
	auto *layout = new QHBoxLayout(this);
	layout->setSpacing(spacing);
	layout->setContentsMargins(spacing, 0, spacing, 0);

	icon = new QLabel(this);
	icon->setVisible(false);

	info = new QLabel(this);
	info->setToolTip("Currently playing from");
	info->setVisible(false);

	layout->addWidget(icon);
	layout->addWidget(info, 1);

	info->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(info, &QWidget::customContextMenuRequested,
		this, &View::Context::Title::onMenu);
}

void View::Context::Title::onMenu(const QPoint &pos)
{
	if (info == nullptr)
	{
		return;
	}

	auto *menu = new QMenu(info);

	if (lib::developer_mode::enabled)
	{
		const auto uri = QString::fromStdString(current.playback.context.uri);
		auto *devContext = menu->addAction(uri);
		devContext->setEnabled(false);
	}

	auto openIcon = icon == nullptr
		? getIcon()
		: QIcon(icon->pixmap(Qt::ReturnByValue));

	auto openText = QString("Open %1")
		.arg(QString::fromStdString(current.playback.context.type));

	auto *open = menu->addAction(openIcon, openText);
	QAction::connect(open, &QAction::triggered,
		this, &View::Context::Title::onInfoOpen);

	menu->popup(info->mapToGlobal(pos));
}

void View::Context::Title::onInfoOpen(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	const auto &type = current.playback.context.type;
	const auto uri = lib::strings::split(current.playback.context.uri, ':').back();

	if (type == "album")
	{
		mainWindow->loadAlbum(uri);
	}
	else if (type == "artist")
	{
		mainWindow->openArtist(uri);
	}
	else if (type == "playlist")
	{
		spotify.playlist(uri, [mainWindow](const lib::spt::playlist &playlist)
		{
			mainWindow->resetLibraryPlaylist();
			mainWindow->getSongsTree()->load(playlist);
		});
	}
}

auto View::Context::Title::getIcon() const -> QIcon
{
	return Icon::get(QString("view-media-%1")
		.arg(current.playback.context.type.empty()
			? "track"
			: current.playback.context.type == "album"
				? "album-cover"
				: QString::fromStdString(current.playback.context.type)));
}

void View::Context::Title::updateIcon()
{
	auto callback = [this](const std::string &currentName)
	{
		auto size = 0;
		if (this->info != nullptr)
		{
			this->info->setText(QString::fromStdString(currentName));
			size = this->info->fontInfo().pixelSize();
		}

		auto *mainWindow = currentName == "No context"
			? MainWindow::find(parentWidget())
			: nullptr;

		auto currentTrack = -1;
		auto totalTracks = -1;

		if (mainWindow != nullptr)
		{
			const auto &uris = mainWindow->getQueue();
			if (uris.empty())
			{
				return;
			}

			const auto trackUri = lib::spt::api::to_uri("track", current.playback.item.id);
			const auto trackIndex = lib::vector::index_of(uris, trackUri);

			currentTrack = static_cast<int>(trackIndex);
			totalTracks = static_cast<int>(uris.size());
		}

		auto fromQueue = currentTrack >= 0 && totalTracks >= 0;
		auto show = currentName != "No context" || fromQueue;

		if (this->icon != nullptr)
		{
			auto newIcon = fromQueue
				? Icon::get("view-media-track")
				: getIcon();

			this->icon->setPixmap(newIcon.pixmap(size, size));
			this->icon->setVisible(show);
		}
		if (this->info != nullptr)
		{
			this->info->setVisible(show);

			if (fromQueue)
			{
				this->info->setText(QString("Track %1/%2")
					.arg(currentTrack + 1)
					.arg(totalTracks));
			}
		}
	};

	if (current.playback.context.type.empty()
		|| current.playback.context.uri.empty())
	{
		callback("No context");
	}
	else if (current.playback.context.type == "album")
	{
		callback(current.playback.item.album.name);
	}
	else if (current.playback.context.type == "artist")
	{
		auto name = current.playback.item.artists.empty()
			? std::string()
			: current.playback.item.artists.front().name;

		auto id = lib::spt::api::to_id(current.playback.context.uri);
		for (const auto &artist : current.playback.item.artists)
		{
			if (artist.id == id)
			{
				name = artist.name;
			}
		}

		callback(name);
	}
	else
	{
		playlistName(current.playback.context.uri, callback);
	}
}

auto View::Context::Title::playlist(const std::string &id) -> lib::spt::playlist
{
	for (const auto &playlist : cache.get_playlists())
	{
		if (lib::strings::ends_with(id, playlist.id))
		{
			return playlist;
		}
	}
	return lib::spt::playlist();
}

auto View::Context::Title::playlistNameFromSaved(const std::string &id) -> std::string
{
	return playlist(id).name;
}

void View::Context::Title::playlistName(const std::string &id, lib::callback<std::string> &callback)
{
	const auto &name = playlistNameFromSaved(id);
	if (!name.empty())
	{
		callback(name);
	}
	else
	{
		spotify.playlist(lib::spt::api::to_id(id),
			[callback](const lib::spt::playlist &playlist)
			{
				callback(playlist.name);
			});
	}
}
