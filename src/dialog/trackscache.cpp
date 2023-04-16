#include "dialog/trackscache.hpp"

Dialog::TracksCache::TracksCache(lib::cache &cache, QWidget *parent)
	: Base(parent),
	cache(cache)
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

void Dialog::TracksCache::setPlaylistId(const std::string &value)
{
	playlistId = value;
}

void Dialog::TracksCache::showEvent(QShowEvent *event)
{
	tree->clear();
	const auto allTracks = cache.all_tracks();

	if (!playlistId.empty())
	{
		if (allTracks.find(playlistId) != allTracks.cend())
		{
			for (const auto &track: allTracks.at(playlistId))
			{
				addTrack(track);
			}
		}
	}
	else
	{
		for (const auto &pair: cache.all_tracks())
		{
			for (const auto &track: pair.second)
			{
				addTrack(track);
			}
		}
	}

	QDialog::showEvent(event);
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
