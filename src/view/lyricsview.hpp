#pragma once

#include "lib/httpclient.hpp"
#include "lib/spotify/track.hpp"
#include "lib/lyrics.hpp"

#include <QDockWidget>
#include <QTextEdit>

class LyricsView: public QDockWidget
{
Q_OBJECT

public:
	LyricsView(const lib::http_client &httpClient, QWidget *parent);

	void open(const lib::spt::track &track);

private:
	const lib::http_client &http;
	QTextEdit *lyricsView = nullptr;
};
