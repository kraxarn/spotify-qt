#pragma once

#include "lib/developermode.hpp"
#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include "spotify/current.hpp"
#include "menu/songmenu.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>

class ContextView: public QWidget
{
public:
	ContextView(lib::spt::api &spotify, const lib::settings &settings,
		spt::Current &current, const lib::cache &cache, QWidget *parent);

	void setAlbum(const QPixmap &pixmap);

	void reset();

	auto getCurrentlyPlaying() const -> const lib::spt::track &;
	void setCurrentlyPlaying(const lib::spt::track &track);

	void updateContextIcon();

private:
	QLabel *contextIcon = nullptr;
	QLabel *contextInfo = nullptr;

	QLabel *nowAlbum = nullptr;
	QLabel *nowPlaying = nullptr;

	lib::spt::api &spotify;
	const lib::settings &settings;
	spt::Current &current;
	const lib::cache &cache;
	lib::spt::track currentlyPlaying;

	void onContextInfoMenu(const QPoint &pos);
	void onContextInfoOpen(bool checked);

	void onSongMenu(const QPoint &pos);

	auto currentContextIcon() const -> QIcon;

	auto playlist(const std::string &id) -> lib::spt::playlist;
	auto playlistNameFromSaved(const std::string &id) -> std::string;
	void playlistName(const std::string &id, lib::callback<std::string> &callback);
};
