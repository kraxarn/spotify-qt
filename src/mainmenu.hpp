#pragma once

#include "mainwindow.hpp"
#include "spotify/spotify.hpp"

#include <QMenu>
#include <QMessageBox>

class MainMenu : public QMenu
{
	Q_OBJECT

public:
	MainMenu(spt::Spotify &spotify, Settings &settings, QWidget *parent = nullptr);

private:
	QWidget			*parent;
	spt::Spotify	&spotify;
	Settings		&settings;
	QAction *createMenuAction(const QString &iconName, const QString &text,
		QKeySequence::StandardKey shortcut = QKeySequence::UnknownKey);
	void refreshDevices(QMenu *deviceMenu);
};
