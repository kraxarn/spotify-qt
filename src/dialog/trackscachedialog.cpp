#include "trackscachedialog.hpp"

TracksCacheDialog::TracksCacheDialog(QWidget *parent)
	: QDialog(parent)
{
	resize(400, 300);
	setWindowTitle("Tracks cache");

	auto layout = new QVBoxLayout(this);
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

	auto buttons = new QDialogButtonBox(this);
	layout->addWidget(buttons);
	QPushButton::connect(buttons->addButton(QDialogButtonBox::Ok),
		&QPushButton::clicked, this, &TracksCacheDialog::okClicked);
}

void TracksCacheDialog::okClicked(bool)
{
	accept();
}

void TracksCacheDialog::open()
{
	tree->clear();

	QtPaths paths(this);
	lib::cache cache(paths);

	for (const auto &pair : cache.all_tracks())
	{
		for (auto &track : pair.second)
		{
			new QTreeWidgetItem(tree, {
				QString::fromStdString(track.name),
				QString::fromStdString(track.artist),
				QString::fromStdString(track.album),
			});
		}
	}

	QDialog::open();
}
