#pragma once

#include "lib/settings.hpp"
#include "lib/spotify/request.hpp"

#include <QObject>

class Refresher: public QObject
{
Q_OBJECT

public:
	Refresher(lib::settings &settings, lib::spt::request &spotify);

	auto refresh() -> bool;

private:
	lib::settings &settings;
	lib::spt::request &spotify;
};
