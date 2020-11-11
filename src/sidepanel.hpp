#pragma once

#include "artistview.hpp"
#include "searchview.hpp"

#include <QTabWidget>

class SidePanel: public QTabWidget
{
Q_OBJECT

public:
	explicit SidePanel(spt::Spotify &spotify, const Settings &settings, QWidget *parent);

	void openArtist(const QString &artistId);
	void openSearch();
	void closeSearch();

private:
	QWidget *parent = nullptr;
	spt::Spotify &spotify;
	const Settings &settings;

	QWidget *artistView = nullptr;
	QWidget *searchView = nullptr;

	void tabRemoved(int index) override;
};
