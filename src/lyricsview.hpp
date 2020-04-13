#pragma once

class LyricsView;

#include "mainwindow.hpp"

#include <QDockWidget>

class LyricsView : public QDockWidget
{
	Q_OBJECT

public:
	LyricsView(const QString &artist, const QString &name, QWidget *parent);

	bool lyricsFound();

private:
	bool found;
};


