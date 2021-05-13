#pragma once

#include "view/artistview.hpp"
#include "view/searchview.hpp"
#include "view/audiofeaturesview.hpp"
#include "lib/spotify/track.hpp"
#include "view/lyricsview.hpp"

#include <QTabWidget>

class SidePanel: public QTabWidget
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

private:
	QWidget *parent = nullptr;
	QWidget *searchView = nullptr;

	spt::Spotify &spotify;
	const lib::settings &settings;
	lib::cache &cache;
	const lib::http_client &httpClient;

	void tabRemoved(int index) override;
	void addAndSelect(QWidget *widget, const QString &icon, const QString &title);
};
