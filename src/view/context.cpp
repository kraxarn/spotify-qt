#include "view/context.hpp"

#include "mainwindow.hpp"

View::Context::Context(lib::spt::api &spotify, const lib::settings &settings,
	spt::Current &current, const lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	settings(settings),
	current(current),
	cache(cache),
	QWidget(parent)
{
	auto *layout = new QVBoxLayout(this);

	// Current context info
	auto *contextLayout = new QHBoxLayout();
	contextIcon = new QLabel(this);
	contextIcon->setVisible(false);

	contextInfo = new QLabel(this);
	contextInfo->setToolTip("Currently playing from");
	contextInfo->setVisible(false);

	contextLayout->addSpacing(16);
	contextLayout->addWidget(contextIcon);
	contextLayout->addWidget(contextInfo, 1);
	layout->addLayout(contextLayout);

	contextInfo->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(contextInfo, &QWidget::customContextMenuRequested,
		this, &View::Context::onContextInfoMenu);

	// Now playing song
	auto *nowPlayingLayout = new QHBoxLayout();
	nowPlayingLayout->setSpacing(12);
	nowAlbum = new QLabel(this);
	nowAlbum->setFixedSize(64, 64);

	nowPlayingLayout->addWidget(nowAlbum);
	nowPlaying = new QLabel(this);
	nowPlaying->setWordWrap(true);
	nowPlayingLayout->addWidget(nowPlaying);
	layout->addLayout(nowPlayingLayout);

	reset();

	// Show menu when clicking now playing
	nowPlaying->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(nowPlaying, &QWidget::customContextMenuRequested,
		this, &View::Context::onSongMenu);
}

void View::Context::onContextInfoMenu(const QPoint &pos)
{
	if (contextInfo == nullptr)
	{
		return;
	}

	auto *menu = new QMenu(contextInfo);

	if (lib::developer_mode::enabled)
	{
		auto *devContext = menu->addAction(current.context);
		devContext->setEnabled(false);
	}

	auto *open = menu->addAction(currentContextIcon(), QString("Open %1")
		.arg(QString::fromStdString(current.playback.context.type)));
	QAction::connect(open, &QAction::triggered, this, &View::Context::onContextInfoOpen);

	menu->popup(contextInfo->mapToGlobal(pos));
}

void View::Context::onContextInfoOpen(bool /*checked*/)
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

void View::Context::onSongMenu(const QPoint &pos)
{
	auto track = current.playback.item;
	if (track.name.empty()
		&& track.artists.empty()
		&& nowPlaying != nullptr)
	{
		return;
	}

	auto *menu = new SongMenu(track, spotify, cache, parentWidget());
	menu->popup(nowPlaying->mapToGlobal(pos));
}

auto View::Context::currentContextIcon() const -> QIcon
{
	return Icon::get(QString("view-media-%1")
		.arg(current.playback.context.type.empty()
			? "track"
			: current.playback.context.type == "album"
				? "album-cover"
				: QString::fromStdString(current.playback.context.type)));
}

void View::Context::updateContextIcon()
{
	if (!settings.general.show_context_info
		&& contextIcon != nullptr
		&& contextInfo != nullptr)
	{
		contextIcon->setVisible(false);
		contextInfo->setVisible(false);
		return;
	}

	auto callback = [this](const std::string &currentName)
	{
		auto size = 0;
		if (contextInfo != nullptr)
		{
			contextInfo->setText(QString::fromStdString(currentName));
			size = this->contextInfo->fontInfo().pixelSize();
		}

		auto show = currentName != "No context";

		if (this->contextIcon != nullptr)
		{
			this->contextIcon->setPixmap(currentContextIcon().pixmap(size, size));
			this->contextIcon->setVisible(show);
		}
		if (this->contextInfo != nullptr)
		{
			this->contextInfo->setVisible(show);
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
		callback(lib::spt::entity::combine_names(current.playback.item.artists));
	}
	else
	{
		playlistName(current.playback.context.uri, callback);
	}
}

auto View::Context::playlist(const std::string &id) -> lib::spt::playlist
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

auto View::Context::playlistNameFromSaved(const std::string &id) -> std::string
{
	return playlist(id).name;
}

void View::Context::playlistName(const std::string &id, lib::callback<std::string> &callback)
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

void View::Context::setAlbum(const QPixmap &pixmap)
{
	if (nowAlbum != nullptr)
	{
		nowAlbum->setPixmap(ImageUtils::mask(pixmap));
	}
}

void View::Context::reset()
{
	if (nowAlbum != nullptr)
	{
		nowAlbum->setPixmap(Icon::get("media-optical-audio").pixmap(nowAlbum->size()));
	}
	if (nowPlaying != nullptr)
	{
		nowPlaying->setText("No music playing");
	}
}

auto View::Context::getCurrentlyPlaying() const -> const lib::spt::track &
{
	return currentlyPlaying;
}

void View::Context::setCurrentlyPlaying(const lib::spt::track &track)
{
	if (nowPlaying != nullptr)
	{
		nowPlaying->setText(QString::fromStdString(track.details()));
	}
	currentlyPlaying = track;
}
