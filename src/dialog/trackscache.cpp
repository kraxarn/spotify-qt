#include "dialog/trackscache.hpp"

Dialog::TracksCache::TracksCache(QWidget *parent)
	: Base(parent)
{
	resize(width, height);
	setWindowTitle(QStringLiteral("Tracks cache"));
	auto *layout = Base::layout<QVBoxLayout>();

	tree = new QTreeWidget(this);
	layout->addWidget(tree);

	tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tree->setSelectionBehavior(QAbstractItemView::SelectRows);
	tree->setSortingEnabled(true);
	tree->setRootIsDecorated(false);
	tree->setAllColumnsShowFocus(true);
	tree->setColumnCount(4);
	tree->setHeaderLabels({
		QStringLiteral("ID"),
		QStringLiteral("Title"),
		QStringLiteral("Artist"),
		QStringLiteral("Album"),
	});

	Base::addAction(DialogAction::Ok);
}

void Dialog::TracksCache::load(const std::vector<lib::spt::track> &tracks)
{
	for (const auto &track: tracks)
	{
		addTrack(track);
	}
}

void Dialog::TracksCache::loadAllTracks(const lib::cache &cache)
{
	for (const auto &pair: cache.all_tracks())
	{
		for (const auto &track: pair.second)
		{
			addTrack(track);
		}
	}
}

void Dialog::TracksCache::addTrack(const lib::spt::track &track)
{
	new QTreeWidgetItem(tree, {
		QString::fromStdString(track.id),
		QString::fromStdString(track.name),
		QString::fromStdString(lib::spt::entity::combine_names(track.artists)),
		QString::fromStdString(track.album.name),
	});
}
