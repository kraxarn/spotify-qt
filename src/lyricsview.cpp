#include "lyricsview.hpp"

LyricsView::LyricsView(const QString &artist, const QString &name, QWidget *parent)
	: QDockWidget(parent)
{
	auto window = (MainWindow *) parent;

	auto reply = window->get(QString("https://lyrics.fandom.com/wiki/%1:%2?action=raw")
		.arg(format(artist))
		.arg(format(name)));
	if (reply.isEmpty())
	{
		window->setStatus("Lyrics not found", true);
		found = false;
		return;
	}

	auto html = QString(reply);
	auto lyricsStart = html.indexOf("<lyrics>") + 9;
	html = html.mid(lyricsStart, html.indexOf("</lyrics>") - lyricsStart);

	setWindowTitle(QString("%1 - %2")
		.arg(artist)
		.arg(name));
	auto lyricsView = new QTextEdit(this);
	lyricsView->setPlainText(html.trimmed());
	lyricsView->setReadOnly(true);
	setWidget(lyricsView);
	setMinimumWidth(300);
	found = true;
}

bool LyricsView::lyricsFound() const
{
	return found;
}

QString LyricsView::format(const QString &word)
{
	auto words = word.split(' ');
	for (auto &w : words)
		w = w.left(1).toUpper() + w.right(w.length() - 1).toLower();
	return words.join('_');
}
