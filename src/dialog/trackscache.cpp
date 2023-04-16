#include "dialog/trackscache.hpp"

Dialog::TracksCache::TracksCache(lib::cache &cache, QWidget *parent)
	: Base(parent),
	cache(cache)
{
	resize(width, height);
	setWindowTitle(QStringLiteral("Tracks cache"));

	auto *layout = new QVBoxLayout(this);
	setLayout(layout);

	tree = new QTreeWidget(this);
	layout->addWidget(tree);

	tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tree->setSelectionBehavior(QAbstractItemView::SelectRows);
	tree->setSortingEnabled(true);
	tree->setRootIsDecorated(false);
	tree->setAllColumnsShowFocus(true);
	tree->setColumnCount(3);
	tree->setHeaderLabels({
		QStringLiteral("Title"),
		QStringLiteral("Artist"),
		QStringLiteral("Album"),
	});

	auto *buttons = new QDialogButtonBox(this);
	layout->addWidget(buttons);

	Base::addAction(DialogAction::Ok);
}

void Dialog::TracksCache::showEvent(QShowEvent *event)
{
	tree->clear();

	for (const auto &pair: cache.all_tracks())
	{
		for (const auto &track: pair.second)
		{
			new QTreeWidgetItem(tree, {
				QString::fromStdString(track.name),
				QString::fromStdString(lib::spt::entity::combine_names(track.artists)),
				QString::fromStdString(track.album.name),
			});
		}
	}

	QDialog::showEvent(event);
}
