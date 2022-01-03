#include "maincontent.hpp"

MainContent::MainContent(lib::spt::api &spotify, lib::settings &settings,
	lib::cache &cache, QWidget *parent)
	: QWidget(parent)
{
	layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	status = new StatusMessage(this);
	layout->addWidget(status);

	tracks = new List::Tracks(spotify, settings, cache, this);
	layout->addWidget(tracks, 1);
}

auto MainContent::getTracksList() const -> List::Tracks *
{
	return tracks;
}
