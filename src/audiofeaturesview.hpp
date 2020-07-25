#pragma once

#include "spotify/spotify.hpp"
#include "util/utils.hpp"

#include <QAbstractItemView>
#include <QDockWidget>
#include <QHeaderView>
#include <QTreeWidget>

class AudioFeaturesView : public QDockWidget
{
	Q_OBJECT

public:
	AudioFeaturesView(
		spt::Spotify &spotify, const QString &trackId,
		const QString &artist, const QString &name, QWidget *parent = nullptr);
};