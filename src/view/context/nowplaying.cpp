#include "nowplaying.hpp"

View::Context::NowPlaying::NowPlaying(QWidget *parent)
	: QLabel(parent)
{
	setWordWrap(true);
}

void View::Context::NowPlaying::setTrack(const lib::spt::track &track)
{
	setText(QString::fromStdString(track.details()));
}

void View::Context::NowPlaying::setNoPlaying()
{
	setText("No music playing");
}
