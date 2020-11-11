#pragma once

#include "artistview.hpp"

#include <QTabWidget>

class SidePanel: public QTabWidget
{
Q_OBJECT

public:
	explicit SidePanel(spt::Spotify &spotify, const Settings &settings, QWidget *parent);

	void openArtist(const QString &artistId);

private:
	QWidget *parent = nullptr;
	spt::Spotify &spotify;
	const Settings &settings;

	QWidget *artistView = nullptr;

	void tabRemoved(int index) override;
};
