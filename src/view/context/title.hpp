#pragma once

#include "lib/developermode.hpp"
#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include "spotify/current.hpp"
#include "util/icon.hpp"
#include "view/context/titleinfo.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>

namespace Context
{
	class Title: public QWidget
	{
	Q_OBJECT

	public:
		Title(lib::spt::api &spotify, const lib::cache &cache, QWidget *parent);

	private:
		QLabel *icon = nullptr;
		TitleInfo *info = nullptr;

		lib::spt::api &spotify;
		const lib::cache &cache;

		static constexpr int spacing = 16;

		auto playlist(const std::string &id) -> lib::spt::playlist;
		auto playlistNameFromSaved(const std::string &id) -> std::string;
		void playlistName(const std::string &id, lib::callback<std::string> &callback);

		void onPlaybackRefreshed(const lib::spt::playback &playback,
			const lib::spt::playback &previous);
	};
}
