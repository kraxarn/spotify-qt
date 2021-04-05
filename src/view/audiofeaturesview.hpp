#pragma once

#include "../spotify/spotify.hpp"
#include "../util/utils.hpp"
#include "lib/spotify/audiofeatures.hpp"

#include <QAbstractItemView>
#include <QDockWidget>
#include <QHeaderView>
#include <QTreeWidget>

class AudioFeaturesView: public QTreeWidget
{
Q_OBJECT

public:
	AudioFeaturesView(spt::Spotify &spotify, const std::string &trackId, QWidget *parent);

private:
	void loaded(const lib::spt::audio_features &features);
};
