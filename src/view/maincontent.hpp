#pragma once

#include "list/trackslist.hpp"

#include <QVBoxLayout>

class MainContent: public QWidget
{
Q_OBJECT

public:
	MainContent(lib::spt::api &spotify, lib::settings &settings,
		lib::cache &cache, QWidget *parent);

	auto getTracksList() const -> TracksList *;

private:
	QVBoxLayout *layout = nullptr;

	TracksList *tracks;
};
