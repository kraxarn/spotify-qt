#include "lyricsview.hpp"

LyricsView::LyricsView(const QString &artist, const QString &name, QWidget *parent) : QDockWidget(parent)
{
	auto window = (MainWindow*) parent;
	window->setStatus("Loading lyrics...");
	auto reply = window->get(QString(
		"https://vscode-spotify-lyrics.azurewebsites.net/lyrics?artist=%1&title=%2")
			.arg(artist)
			.arg(name));
	auto lyricsText = QString(reply).trimmed();
	if (lyricsText == "Not found")
	{
		window->setStatus("Lyrics not found");
		return;
	}
	window->setStatus("Lyrics loaded");
	setWindowTitle(QString("%1 - %2")
		.arg(artist)
		.arg(name));
	auto lyricsView = new QTextEdit(this);
	lyricsView->setHtml(lyricsText.replace("\n", "<br/>"));
	lyricsView->setReadOnly(true);
	setWidget(lyricsView);
	setMinimumWidth(300);
}