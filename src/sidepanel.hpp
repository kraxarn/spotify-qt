#pragma once

#include "artistview.hpp"
#include "searchview.hpp"
#include "audiofeaturesview.hpp"

#include <QTabWidget>

class SidePanel: public QTabWidget
{
Q_OBJECT

public:
	explicit SidePanel(spt::Spotify &spotify, const Settings &settings, QWidget *parent);

	void openArtist(const QString &artistId);
	void openAudioFeatures(const QString &trackId, const QString &artist, const QString &name);
	void openSearch();
	void closeSearch();

private:
	QWidget *parent = nullptr;
	spt::Spotify &spotify;
	const Settings &settings;
	QWidget *searchView = nullptr;

	void tabRemoved(int index) override;
	void addAndSelect(QWidget *widget, const QString &icon, const QString &title);
};
