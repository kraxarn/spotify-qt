#include "view/context/title.hpp"

#include "mainwindow.hpp"

Context::Title::Title(lib::spt::api &spotify, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: QWidget(parent),
	spotify(spotify),
	current(current),
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
}

void Context::Title::updateIcon()
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

		auto id = lib::spt::uri_to_id(current.playback.context.uri);
		for (const auto &artist: current.playback.item.artists)
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
