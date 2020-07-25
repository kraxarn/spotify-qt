#pragma once

#include "../spotify/playlist.hpp"

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

class PlaylistEditDialog : public QDialog
{
	Q_OBJECT

public:
	PlaylistEditDialog(spt::Spotify *spotify, const spt::Playlist &playlist,
		int selectedIndex, QWidget *parent = nullptr);

private:
	QLineEdit	*name;
	QTextEdit	*description;
	QCheckBox	*isPublic, *isCollaborative;
};