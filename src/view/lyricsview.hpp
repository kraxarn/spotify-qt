#pragma once

#include "lib/spotify/api.hpp"
#include "lib/lyrics.hpp"
#include "lib/cache.hpp"

#include <QTextEdit>

class LyricsView: public QTextEdit
{
Q_OBJECT

public:
	LyricsView(lib::spt::api &spotify, lib::cache &cache, QWidget *parent);

	void open(const lib::spt::track &track);

private:
	lib::spt::api &spotify;
	lib::cache &cache;
};
