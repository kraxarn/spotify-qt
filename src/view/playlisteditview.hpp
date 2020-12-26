#pragma once

#include "../spotify/playlist.hpp"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

class PlaylistEditView: public QWidget
{
Q_OBJECT

public:
	PlaylistEditView(spt::Spotify *spotify, const spt::Playlist &playlist,
		int selectedIndex, QWidget *parent = nullptr);

private:
	QLineEdit *name;
	QTextEdit *description;
	QCheckBox *isPublic, *isCollaborative;
};