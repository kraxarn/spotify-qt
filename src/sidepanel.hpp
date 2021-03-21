#pragma once

#include "view/artistview.hpp"
#include "view/searchview.hpp"
#include "view/audiofeaturesview.hpp"

#include <QTabWidget>

class SidePanel: public QTabWidget
{
Q_OBJECT

public:
	explicit SidePanel(spt::Spotify &spotify, const lib::settings &settings, QWidget *parent);

	void openArtist(const std::string &artistId);
	void openAudioFeatures(const std::string &trackId, const std::string &artist,
		const std::string &name);
	void openSearch();
	void closeSearch();

private:
	QWidget *parent = nullptr;
	spt::Spotify &spotify;
	const lib::settings &settings;
	QWidget *searchView = nullptr;

	void tabRemoved(int index) override;
	void addAndSelect(QWidget *widget, const QString &icon, const QString &title);
};
