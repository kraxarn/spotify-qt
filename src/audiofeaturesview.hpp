#pragma once

#include "spotify/spotify.hpp"

#include <QAbstractItemView>
#include <QDockWidget>
#include <QHeaderView>
#include <QTreeWidget>

class AudioFeaturesView : public QDockWidget
{
	Q_OBJECT
public:
	AudioFeaturesView(spt::Spotify &spotify, const QString &trackId, const QString &artist, const QString &name, QWidget *parent = nullptr);

private:
	QTreeWidgetItem *treeItem(QTreeWidget *tree, const QString &key, const QString &value);
};