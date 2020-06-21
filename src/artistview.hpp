#pragma once

#include "mainwindow.hpp"
#include "songmenu.hpp"
#include "spotify/spotify.hpp"

#include <QDockWidget>
#include <QLabel>
#include <QListWidget>
#include <QTabWidget>
#include <QToolTip>
#include <QTreeWidget>
#include <QVBoxLayout>

class ArtistView : public QDockWidget
{
	Q_OBJECT

public:
	ArtistView(spt::Spotify *spotify, const QString &artistId, QWidget *parent);
};