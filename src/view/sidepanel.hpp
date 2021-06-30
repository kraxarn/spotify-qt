#pragma once

#include "lib/spotify/track.hpp"
#include "view/artistview.hpp"
#include "view/searchview.hpp"
#include "view/audiofeaturesview.hpp"
#include "view/lyricsview.hpp"
#include "view/sidepaneltitle.hpp"

#include <QTabWidget>
#include <QStackedWidget>

class SidePanel: public QDockWidget
{
Q_OBJECT

public:
	explicit SidePanel(spt::Spotify &spotify, const lib::settings &settings, lib::cache &cache,
		const lib::http_client &httpClient, QWidget *parent);

	void openArtist(const std::string &artistId);
	void openAudioFeatures(const std::string &trackId, const std::string &artist,
		const std::string &name);
	void openLyrics(const lib::spt::track &track);
	void openSearch();
	void closeSearch();

	void addTab(QWidget *widget, const QString &icon, const QString &tabTitle);

private:
	View::SidePanelTitle *title = nullptr;
	QStackedWidget *stack = nullptr;

	QWidget *parent = nullptr;
	QWidget *searchView = nullptr;

	spt::Spotify &spotify;
	const lib::settings &settings;
	lib::cache &cache;
	const lib::http_client &httpClient;

	void removeTab(int index);
};
