#include "playlistspage.hpp"

PlaylistsPage::PlaylistsPage(lib::settings &settings, QWidget *parent)
	: SettingsPage(settings, parent)
{
	addTab(order(), "Order");
}

QWidget *PlaylistsPage::order()
{
	auto layout = new QVBoxLayout();

	plHints = QStringList({
		"Display in the order they are fetched.",
		"Display in alphabetical order from A to Z.",
		"Display in order of most recently edited.",
		"Display in a custom order defined below."
	});

	auto typeContainer = new QHBoxLayout();
	typeContainer->addWidget(new QLabel("Playlist order"));
	plOrder = new QComboBox(this);
	plOrder->addItems({
		"Default", "Alphabetical", "Recent", "Custom"
	});
	plOrder->setCurrentIndex(settings.general.playlist_order);
	typeContainer->addWidget(plOrder);
	layout->addLayout(typeContainer);

	plHint = new QLabel(plHints.at(plOrder->currentIndex()), this);
	plHint->setWordWrap(true);
	layout->addWidget(plHint);

	QComboBox::connect(plOrder, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, &PlaylistsPage::playlistOrderChanged);

	auto mainWindow = MainWindow::find(parentWidget());

	plListLayout = new QHBoxLayout();
	plList = new QListWidget(this);
	plListLayout->addWidget(plList, 1);
	QListWidget::connect(plList, &QListWidget::currentRowChanged, this,
		&PlaylistsPage::playlistItemChanged);

	auto buttons = new QToolBar(this);
	buttons->setOrientation(Qt::Vertical);

	plBtnUp = buttons->addAction(Icon::get("go-up"), "Up");
	plBtnUp->setEnabled(false);
	QAction::connect(plBtnUp, &QAction::triggered, this, &PlaylistsPage::playlistUp);

	plBtnDown = buttons->addAction(Icon::get("go-down"), "Down");
	plBtnDown->setEnabled(false);
	QAction::connect(plBtnDown, &QAction::triggered, this, &PlaylistsPage::playlistDown);

	plListLayout->addWidget(buttons);
	layout->addLayout(plListLayout, 1);
	playlistOrderChanged(plOrder->currentIndex());

	for (auto i = 0; i < mainWindow->getPlaylistItemCount(); i++)
	{
		auto mainItem = mainWindow->getPlaylistItem(i);
		auto listItem = new QListWidgetItem(mainItem->text());
		listItem->setData(DataRole::RolePlaylistId, mainItem->data(DataRole::RolePlaylistId));
		plList->addItem(listItem);
	}

	return Utils::layoutToWidget(layout);
}

QIcon PlaylistsPage::icon()
{
	return Icon::get("view-media-playlist");
}

QString PlaylistsPage::title()
{
	return "Playlists";
}

bool PlaylistsPage::save()
{
	// Custom playlist order
	auto playlistOrder = (lib::playlist_order) plOrder->currentIndex();
	if (playlistOrder == lib::playlist_order_custom)
	{
		std::vector<std::string> order;
		for (auto i = 0; i < plList->count(); i++)
			order.push_back(plList->item(i)->data(RolePlaylistId).toString().toStdString());
		settings.general.custom_playlist_order = order;
	}

	// Playlist stuff
	auto mainWindow = MainWindow::find(parentWidget());
	if ((settings.general.playlist_order != playlistOrder
		|| playlistOrder == lib::playlist_order_custom)
		&& mainWindow != nullptr)
		mainWindow->orderPlaylists(playlistOrder);
	settings.general.playlist_order = playlistOrder;

	return true;
}

void PlaylistsPage::playlistOrderChanged(int index)
{
	for (auto i = 0; i < plListLayout->count(); i++)
		plListLayout->itemAt(i)->widget()->setEnabled(index == 3);

	plHint->setText(plHints.at(index));
}

void PlaylistsPage::playlistMove(int steps)
{
	auto row = plList->currentRow();
	auto item = plList->takeItem(row);
	plList->insertItem(row + steps, item);
	plList->setCurrentItem(item);
}

void PlaylistsPage::playlistUp()
{
	playlistMove(-1);
}

void PlaylistsPage::playlistDown()
{
	playlistMove(1);
}

void PlaylistsPage::playlistItemChanged(int row)
{
	plBtnUp->setEnabled(row > 0);
	plBtnDown->setEnabled(row < plList->count() - 1);
}