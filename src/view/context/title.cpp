#include "view/context/title.hpp"

#include "mainwindow.hpp"

Context::Title::Title(lib::spt::api &spotify,
	const lib::cache &cache, QWidget *parent)
	: QWidget(parent),
	spotify(spotify),
	cache(cache)
{
	auto *layout = new QHBoxLayout(this);
	layout->setSpacing(spacing);
	layout->setContentsMargins(spacing, 0, spacing, 0);

	icon = new QLabel(this);
	icon->setVisible(false);
	layout->addWidget(icon);

	info = new Context::TitleInfo(spotify, this);
	layout->addWidget(info, 1);

	auto *mainWindow = MainWindow::find(parent);
	MainWindow::connect(mainWindow, &MainWindow::playbackRefreshed,
		this, &Context::Title::onPlaybackRefreshed);
}

void Context::Title::onPlaybackRefreshed(const lib::spt::playback &playback,
	const lib::spt::playback &/*previous*/)
{
	auto callback = [this](const std::string &currentName)
	{
		auto size = 0;
		if (this->info != nullptr)
		{
			this->info->setText(QString::fromStdString(currentName));
			size = this->info->fontInfo().pixelSize();
		}

		auto show = currentName != "No context";

		if (this->icon != nullptr && this->info != nullptr)
		{
			this->icon->setPixmap(info->getIcon().pixmap(size, size));
			this->icon->setVisible(show);
		}
		if (this->info != nullptr)
		{
			this->info->setVisible(show);
		}
	};

	if (playback.context.type.empty()
		|| playback.context.uri.empty())
	{
		callback("No context");
	}
	else if (playback.context.type == "album")
	{
		callback(playback.item.album.name);
	}
	else if (playback.context.type == "artist")
	{
		auto name = playback.item.artists.empty()
			? std::string()
			: playback.item.artists.front().name;

		auto id = lib::spt::uri_to_id(playback.context.uri);
		for (const auto &artist: playback.item.artists)
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
		playlistName(playback.context.uri, callback);
	}
}

auto Context::Title::playlist(const std::string &id) -> lib::spt::playlist
{
	for (const auto &playlist: cache.get_playlists())
	{
		if (lib::strings::ends_with(id, playlist.id))
		{
			return playlist;
		}
	}

	return {};
}

auto Context::Title::playlistNameFromSaved(const std::string &id) -> std::string
{
	return playlist(id).name;
}

void Context::Title::playlistName(const std::string &id, lib::callback<std::string> &callback)
{
	const auto &name = playlistNameFromSaved(id);
	if (!name.empty())
	{
		callback(name);
	}
	else
	{
		spotify.playlist(lib::spt::uri_to_id(id),
			[callback](const lib::spt::playlist &playlist)
			{
				callback(playlist.name);
			});
	}
}
