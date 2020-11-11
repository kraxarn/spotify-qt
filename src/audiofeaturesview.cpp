#include "audiofeaturesview.hpp"

AudioFeaturesView::AudioFeaturesView(spt::Spotify &spotify, const QString &trackId, QWidget *parent)
	: QTreeWidget(parent)
{
	auto features = spotify.trackAudioFeatures(trackId);

	setEditTriggers(QAbstractItemView::NoEditTriggers);
	header()->hide();
	setSelectionMode(QAbstractItemView::NoSelection);
	setRootIsDecorated(false);
	setAllColumnsShowFocus(true);
	setColumnCount(2);
	header()->setSectionResizeMode(QHeaderView::ResizeToContents);

	QMapIterator<QString, QString> i(features.values);
	while (i.hasNext())
	{
		i.next();
		addTopLevelItem(Utils::treeItem(this, i.key(), i.value()));
	}
}