#include "audiofeaturesview.hpp"

AudioFeaturesView::AudioFeaturesView(spt::Spotify &spotify, const QString &trackId, const QString &artist, const QString &name, QWidget *parent) : QDockWidget(parent)
{
	hide();
	spotify.trackAudioFeatures(trackId, [this](const spt::AudioFeatures &features) {
		auto tree = new QTreeWidget(this);
		tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
		tree->header()->hide();
		tree->setSelectionMode(QAbstractItemView::NoSelection);
		tree->setRootIsDecorated(false);
		tree->setAllColumnsShowFocus(true);
		tree->setColumnCount(2);
		tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
		QMapIterator<QString, QString> i(features.values);
		while (i.hasNext())
		{
			i.next();
			tree->addTopLevelItem(treeItem(tree, i.key(), i.value()));
		}
		setWidget(tree);
		show();
	});
	setWindowTitle(QString("%1 - %2")
		.arg(artist)
		.arg(name)
		.replace("&", "&&"));
	setMinimumWidth(150);
}

QTreeWidgetItem *AudioFeaturesView::treeItem(QTreeWidget *tree, const QString &key, const QString &value)
{
	return new QTreeWidgetItem(tree, {
		key, value
	});
}