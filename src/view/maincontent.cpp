#include "maincontent.hpp"

MainContent::MainContent(lib::spt::api &spotify, lib::settings &settings,
	lib::cache &cache, QWidget *parent)
	: QWidget(parent)
{
	layout = new QVBoxLayout(this);

	tracks = new TracksList(spotify, settings, cache, this);
	layout->addWidget(tracks, 1);
}

auto MainContent::getTracksList() const -> TracksList *
{
	return tracks;
}
