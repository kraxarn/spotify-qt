#pragma once

#include "lib/spotify/playlist.hpp"

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>

class PlaylistEditDialog: public QDialog
{
Q_OBJECT

public:
	PlaylistEditDialog(lib::spt::spotify *spotify, const lib::spt::playlist &playlist,
		int selectedIndex, QWidget *parent = nullptr);

private:
	QLineEdit *name;
	QTextEdit *description;
	QCheckBox *isPublic, *isCollaborative;
};