#include "settingspage/playlists.hpp"

#include "mainwindow.hpp"
#include "metatypes.hpp"
#include "util/widget.hpp"

SettingsPage::Playlists::Playlists(lib::settings &settings, QWidget *parent)
	: SettingsPage::Base(settings, parent)
{
	addTab(order(), "Order");
}

auto SettingsPage::Playlists::order() -> QWidget *
{
	auto *layout = new QVBoxLayout();

	plHints = QStringList({
		"Display in the order they are fetched.",
		"Display in alphabetical order from A to Z.",
		"Display in order of most recently edited.",
		"Display in a custom order defined below."
	});

	auto *typeContainer = new QHBoxLayout();
	typeContainer->addWidget(new QLabel("Playlist order"));
	plOrder = new QComboBox(this);
	plOrder->addItems({
		"Default", "Alphabetical", "Recent", "Custom"
	});
	plOrder->setCurrentIndex(static_cast<int>(settings.general.playlist_order));
	typeContainer->addWidget(plOrder);
	layout->addLayout(typeContainer);

	plHint = new QLabel(plHints.at(plOrder->currentIndex()), this);
	plHint->setWordWrap(true);
	layout->addWidget(plHint);

	QComboBox::connect(plOrder, QOverload<int>::of(&QComboBox::currentIndexChanged),
		this, &SettingsPage::Playlists::playlistOrderChanged);

	auto *mainWindow = MainWindow::find(parentWidget());

	plListLayout = new QHBoxLayout();
	plList = new QListWidget(this);
	plListLayout->addWidget(plList, 1);
	QListWidget::connect(plList, &QListWidget::currentRowChanged, this,
		&SettingsPage::Playlists::playlistItemChanged);

	auto *buttons = new QToolBar(this);
	buttons->setOrientation(Qt::Vertical);

	plBtnUp = buttons->addAction(Icon::get("go-up"), "Up");
	plBtnUp->setEnabled(false);
	QAction::connect(plBtnUp, &QAction::triggered, this, &SettingsPage::Playlists::playlistUp);

	plBtnDown = buttons->addAction(Icon::get("go-down"), "Down");
	plBtnDown->setEnabled(false);
	QAction::connect(plBtnDown, &QAction::triggered, this, &SettingsPage::Playlists::playlistDown);

	plListLayout->addWidget(buttons);
	layout->addLayout(plListLayout, 1);
	playlistOrderChanged(plOrder->currentIndex());

	for (auto i = 0; i < mainWindow->getPlaylistItemCount(); i++)
	{
		auto *mainItem = mainWindow->getPlaylistItem(i);
		const auto &mainData = mainItem->data(static_cast<int>(DataRole::Playlist));

		auto *listItem = new QListWidgetItem(mainItem->text());
		listItem->setData(static_cast<int>(DataRole::Playlist), mainData);
		plList->addItem(listItem);
	}

	return Widget::layoutToWidget(layout, this);
}

auto SettingsPage::Playlists::icon() -> QIcon
{
	return Icon::get("view-media-playlist");
}

auto SettingsPage::Playlists::title() -> QString
{
	return "Playlists";
}

auto SettingsPage::Playlists::save() -> bool
{
	// Custom playlist order
	auto playlistOrder = static_cast<lib::playlist_order>(plOrder->currentIndex());
	if (playlistOrder == lib::playlist_order::custom)
	{
		std::vector<std::string> order;
		order.reserve(plList->count());
		for (auto i = 0; i < plList->count(); i++)
		{
			const auto &playlistData = plList->item(i)->data(static_cast<int>(DataRole::Playlist));
			const auto playlist = playlistData.value<lib::spt::playlist>();
			order.push_back(playlist.id);
		}
		settings.general.custom_playlist_order = order;
	}

	// Playlist stuff
	auto *mainWindow = MainWindow::find(parentWidget());
	if ((settings.general.playlist_order != playlistOrder
		|| playlistOrder == lib::playlist_order::custom)
		&& mainWindow != nullptr)
	{
		mainWindow->orderPlaylists(playlistOrder);
	}
	settings.general.playlist_order = playlistOrder;

	return true;
}

void SettingsPage::Playlists::playlistOrderChanged(int index)
{
	for (auto i = 0; i < plListLayout->count(); i++)
	{
		plListLayout->itemAt(i)->widget()->setEnabled(index == 3);
	}

	plHint->setText(plHints.at(index));
}

void SettingsPage::Playlists::playlistMove(int steps)
{
	auto row = plList->currentRow();
	auto *item = plList->takeItem(row);
	plList->insertItem(row + steps, item);
	plList->setCurrentItem(item);
}

void SettingsPage::Playlists::playlistUp()
{
	playlistMove(-1);
}

void SettingsPage::Playlists::playlistDown()
{
	playlistMove(1);
}

void SettingsPage::Playlists::playlistItemChanged(int row)
{
	plBtnUp->setEnabled(row > 0);
	plBtnDown->setEnabled(row < plList->count() - 1);
}
