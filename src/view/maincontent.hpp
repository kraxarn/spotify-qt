#pragma once

#include "list/tracks.hpp"
#include "widget/statusmessage.hpp"

#include <QVBoxLayout>

class MainContent: public QWidget
{
Q_OBJECT

public:
	MainContent(lib::spt::api &spotify, lib::settings &settings,
		lib::cache &cache, QWidget *parent);

	auto getTracksList() const -> List::Tracks *;

private:
	QVBoxLayout *layout = nullptr;

	List::Tracks *tracks = nullptr;
	StatusMessage *status = nullptr;
};
