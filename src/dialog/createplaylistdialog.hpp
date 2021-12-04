#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>

class CreatePlaylistDialog : public QDialog
{
Q_OBJECT

public:
	CreatePlaylistDialog(QWidget *parent);

	auto playlistName() -> std::string;
	auto playlistDescription() -> std::string;
	auto playlistPublic() -> bool;
	auto playlistCollaborative() -> bool;

private:
	QLineEdit *name_editfield;
	QLineEdit *description_editfield;
	QCheckBox *public_checkbox;
	QCheckBox *collaborative_checkbox;
};
