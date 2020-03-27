#pragma once

#include "mainwindow.hpp"

#include <QDockWidget>


class LyricsView : public QDockWidget
{
	Q_OBJECT
public:
	LyricsView(const QString &artist, const QString &name, QWidget *parent);
};


