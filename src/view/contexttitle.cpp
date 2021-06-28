#include "contexttitle.hpp"

#include "mainwindow.hpp"

View::ContextTitle::ContextTitle(lib::spt::api &spotify, const lib::settings &settings,
	spt::Current &current, const lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	settings(settings),
	current(current),
	cache(cache),
	QWidget(parent)
{
	auto *layout = new QHBoxLayout(this);
	icon = new QLabel(this);
	icon->setVisible(false);

	info = new QLabel(this);
	info->setToolTip("Currently playing from");
	info->setVisible(false);

	layout->addSpacing(16);
	layout->addWidget(icon);
	layout->addWidget(info, 1);

	info->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(info, &QWidget::customContextMenuRequested,
		this, &View::ContextTitle::onMenu);
}

void View::ContextTitle::onMenu(const QPoint &pos)
{
	if (info == nullptr)
	{
		return;
	}

	auto *menu = new QMenu(info);

	if (lib::developer_mode::enabled)
	{
		auto *devContext = menu->addAction(current.context);
		devContext->setEnabled(false);
	}

	auto *open = menu->addAction(getIcon(), QString("Open %1")
		.arg(QString::fromStdString(current.playback.context.type)));
	QAction::connect(open, &QAction::triggered,
		this, &View::ContextTitle::onInfoOpen);

	menu->popup(info->mapToGlobal(pos));
}

void View::ContextTitle::onInfoOpen(bool /*checked*/)
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

auto View::ContextTitle::getIcon() const -> QIcon
{
	return Icon::get(QString("view-media-%1")
		.arg(current.playback.context.type.empty()
			? "track"
			: current.playback.context.type == "album"
				? "album-cover"
				: QString::fromStdString(current.playback.context.type)));
}

void View::ContextTitle::updateIcon()
{
	if (!settings.general.show_context_info && icon != nullptr && info != nullptr)
	{
		icon->setVisible(false);
		info->setVisible(false);
		return;
	}

	auto callback = [this](const std::string &currentName)
	{
		auto size = 0;
		if (this->info != nullptr)
		{
			this->info->setText(QString::fromStdString(currentName));
			size = this->info->fontInfo().pixelSize();
		}

		auto show = currentName != "No context";

		if (this->icon != nullptr)
		{
			this->icon->setPixmap(getIcon().pixmap(size, size));
			this->icon->setVisible(show);
		}
		if (this->info != nullptr)
		{
			this->info->setVisible(show);
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

auto View::ContextTitle::playlist(const std::string &id) -> lib::spt::playlist
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

auto View::ContextTitle::playlistNameFromSaved(const std::string &id) -> std::string
{
	return playlist(id).name;
}

void View::ContextTitle::playlistName(const std::string &id, lib::callback<std::string> &callback)
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
