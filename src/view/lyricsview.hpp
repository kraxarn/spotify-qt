#pragma once

#include "lib/httpclient.hpp"
#include "lib/spotify/track.hpp"
#include "lib/lyrics.hpp"
#include "lib/cache.hpp"

#include <QTextEdit>

class LyricsView: public QTextEdit
{
Q_OBJECT

public:
	LyricsView(const lib::http_client &httpClient,
		lib::cache &cache, QWidget *parent);

	void open(const lib::spt::track &track);

private:
	const lib::http_client &http;
	lib::cache &cache;
	QTextEdit *lyricsView = nullptr;
};
