#pragma once

#include "spotify/spotify.hpp"

#include <QDockWidget>
#include <QTreeWidget>
#include <QAbstractItemView>
#include <QHeaderView>

class AudioFeaturesView : public QDockWidget
{
	Q_OBJECT
public:
	AudioFeaturesView(spt::Spotify &spotify, const QString &trackId, const QString &artist, const QString &name, QWidget *parent = nullptr);

private:
	QTreeWidgetItem *treeItem(QTreeWidget *tree, const QString &key, const QString &value);
};