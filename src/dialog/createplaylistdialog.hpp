#pragma once

#include <QDialog>
#include <QLineEdit>

class CreatePlaylistDialog : public QDialog
{
Q_OBJECT

public:
	CreatePlaylistDialog(QWidget *parent);

	auto playlistName() -> std::string;
	auto playlistDescription() -> std::string;

private:
	QLineEdit *name_editfield;
	QLineEdit *description_editfield;
};
