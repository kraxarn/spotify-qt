#include "dialog/editplaylist.hpp"

#include <QMessageBox>

Dialog::EditPlaylist::EditPlaylist(lib::spt::api &spotify, const lib::spt::playlist &playlist,
	int selectedIndex, QWidget *parent)
	: Base(parent),
	spotify(spotify),
	playlist(playlist)
{
	const auto &playlistName = QString::fromStdString(playlist.name);
	const auto &playlistDescription = QString::fromStdString(playlist.description);

	setWindowTitle(playlistName);
	setModal(true);
	auto *layout = Base::layout<QVBoxLayout>();

	// Name
	layout->addWidget(new QLabel("Name:", this));
	name = new QLineEdit(playlistName, this);
	layout->addWidget(name);

	// Description
	layout->addWidget(new QLabel("Description:", this));
	description = new QTextEdit(playlistDescription, this);
	layout->addWidget(description);

	// Toggles
	auto *toggles = new QHBoxLayout();
	toggles->setAlignment(Qt::AlignLeft);
	isPublic = new QCheckBox("Public", this);
	isPublic->setChecked(playlist.is_public);
	toggles->addWidget(isPublic);
	isCollaborative = new QCheckBox("Collaborative", this);
	isCollaborative->setChecked(playlist.collaborative);
	toggles->addWidget(isCollaborative);
	layout->addLayout(toggles);

	// Focus depending on selected index
	if (selectedIndex < 0)
	{
		setFocus();
	}
	else if (selectedIndex == 0)
	{
		name->setFocus();
	}
	else if (selectedIndex == 1)
	{
		description->setFocus();
	}

	// Dialog buttons
	Base::addAction(DialogAction::Ok);
	Base::addAction(DialogAction::Cancel);
}

void Dialog::EditPlaylist::onOk(bool checked)
{
	lib::spt::playlist_details details(playlist);
	details.name = name->text().toStdString();
	details.description = description->toPlainText().toStdString();
	details.is_public = isPublic->isChecked();
	details.collaborative = isCollaborative->isChecked();

	spotify.edit_playlist(playlist.id, details, [this, checked](const std::string &result)
	{
		if (result.empty())
		{
			Base::onOk(checked);
			return;
		}

		QMessageBox::warning(this, "Edit failed",
			QString("Failed to save changes:\n%1")
				.arg(QString::fromStdString(result)));
	});
}
