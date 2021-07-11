#pragma once

#include "lib/spotify/track.hpp"
#include "view/artist/artist.hpp"
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
	void openAudioFeatures(const lib::spt::track &track);
	void openLyrics(const lib::spt::track &track);
	void openSearch();
	void closeSearch();

	void addTab(QWidget *widget, const QString &icon, const QString &tabTitle);
	void removeTab(int index);

private:
	View::SidePanelTitle *title = nullptr;
	QStackedWidget *stack = nullptr;

	QWidget *parent = nullptr;
	QWidget *searchView = nullptr;

	spt::Spotify &spotify;
	const lib::settings &settings;
	lib::cache &cache;
	const lib::http_client &httpClient;

	void setCurrentIndex(int index);
	void setCurrentWidget(QWidget *widget);
};
