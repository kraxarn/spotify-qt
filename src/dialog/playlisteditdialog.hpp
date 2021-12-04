#pragma once

#include "lib/spotify/api.hpp"

#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLineEdit>

class PlaylistEditDialog: public QDialog
{
Q_OBJECT

public:
	PlaylistEditDialog(lib::spt::api &spotify, const lib::spt::playlist &playlist,
		int selectedIndex, QWidget *parent);

private:
	QLineEdit *name;
	QTextEdit *description;
	QCheckBox *isPublic, *isCollaborative;

	lib::spt::api &spotify;
	const lib::spt::playlist &playlist;

	void yes();
	void no();
};
