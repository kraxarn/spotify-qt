#include "createplaylist.hpp"
#include "mainwindow.hpp"

#include <QVBoxLayout>

Dialog::CreatePlaylist::CreatePlaylist(lib::settings &settings, QWidget *parent)
	: Base(settings, parent)
{
	setTitle("Create playlist");

	addActions(DialogAction::Ok);
	addActions(DialogAction::Cancel);

	auto *layout = new QVBoxLayout();
	layout->addWidget(new QLabel("Playlist name", this));

	playlistName = new QLineEdit(this);
	layout->addWidget(playlistName);

	addLayout(layout);
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
