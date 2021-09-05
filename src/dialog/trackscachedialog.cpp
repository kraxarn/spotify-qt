#include "trackscachedialog.hpp"

TracksCacheDialog::TracksCacheDialog(lib::cache &cache, QWidget *parent)
	: cache(cache),
	QDialog(parent)
{
	resize(400, 300);
	setWindowTitle("Tracks cache");

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
		"Title", "Artist", "Album",
	});

	auto *buttons = new QDialogButtonBox(this);
	layout->addWidget(buttons);
	QPushButton::connect(buttons->addButton(QDialogButtonBox::Ok),
		&QPushButton::clicked, this, &TracksCacheDialog::okClicked);
}

void TracksCacheDialog::okClicked(bool /*checked*/)
{
	accept();
}

void TracksCacheDialog::open()
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

	QDialog::open();
}
