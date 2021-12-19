#pragma once

#include "lib/developermode.hpp"
#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include "spotify/current.hpp"
#include "util/icon.hpp"

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
		Title(lib::spt::api &spotify, spt::Current &current,
			const lib::cache &cache, QWidget *parent);

		void updateIcon();

	private:
		QLabel *icon = nullptr;
		QLabel *info = nullptr;

		lib::spt::api &spotify;
		spt::Current &current;
		const lib::cache &cache;

		static constexpr int spacing = 16;

		void onMenu(const QPoint &pos);
		void onInfoOpen(bool checked);

		auto getIcon() const -> QIcon;

		auto playlist(const std::string &id) -> lib::spt::playlist;
		auto playlistNameFromSaved(const std::string &id) -> std::string;
		void playlistName(const std::string &id, lib::callback<std::string> &callback);
	};
}
