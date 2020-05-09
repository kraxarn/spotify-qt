#pragma once

#include "spotify/playlist.hpp"

#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QCheckBox>

class PlaylistEditDialog : public QDialog
{
	Q_OBJECT

public:
	PlaylistEditDialog(const spt::Playlist &playlist, int selectedIndex, QWidget *parent = nullptr);

private:
	QLineEdit	*name;
	QTextEdit	*description;
	QCheckBox	*isPublic, *isCollaborative;
};