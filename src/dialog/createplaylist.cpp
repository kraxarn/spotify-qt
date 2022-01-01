#include "createplaylist.hpp"
#include "mainwindow.hpp"

#include <QVBoxLayout>

Dialog::CreatePlaylist::CreatePlaylist(QWidget *parent)
	: Base(parent)
{
	setTitle("Create playlist");

	auto *layout = Base::layout<QVBoxLayout>();
	layout->addWidget(new QLabel("Playlist name", this));

	playlistName = new QLineEdit(this);
	layout->addWidget(playlistName);

	addAction(DialogAction::Ok);
	addAction(DialogAction::Cancel);
}

void Dialog::CreatePlaylist::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);

	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		playlistName->setPlaceholderText(QString("Playlist #%1")
			.arg(mainWindow->getPlaylistItemCount()));
	}
}
