#include "view/search/albums.hpp"
#include "mainwindow.hpp"

Search::Albums::Albums(lib::spt::api &spotify, lib::cache &cache,
	const lib::http_client &httpClient, QWidget *parent)
	: Search::SearchTabTree({"Title", "Artist"}, parent),
	spotify(spotify),
	cache(cache),
	httpClient(httpClient)
{
	QTreeWidget::connect(this, &QTreeWidget::itemClicked,
		this, &Search::Albums::onItemClicked);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &Search::Albums::onContextMenu);
}

void Search::Albums::add(const lib::spt::album &album)
{
	auto id = QString::fromStdString(album.id);
	auto name = QString::fromStdString(album.name);
	auto artist = QString::fromStdString(album.artist);

	auto *item = new QTreeWidgetItem({
		name, artist
	});

	Http::getAlbumImage(album.image, httpClient, cache, [item](const QPixmap &image)
	{
		if (item != nullptr)
		{
			item->setIcon(0, image);
		}
	});

	item->setData(0, static_cast<int>(DataRole::Album), QVariant::fromValue(album));
	item->setToolTip(0, name);
	item->setToolTip(1, artist);
	addTopLevelItem(item);
}

void Search::Albums::onItemClicked(QTreeWidgetItem *item, int /*column*/)
{
	const auto albumData = item->data(0, static_cast<int>(DataRole::Album));
	const auto album = albumData.value<lib::spt::album>();

	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->loadAlbum(album.id);
}

void Search::Albums::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	const auto albumData = item->data(0, static_cast<int>(DataRole::Album));
	const auto album = albumData.value<lib::spt::album>();
	if (!album.is_valid())
	{
		return;
	}

	auto *albumMenu = new Menu::Album(spotify, cache, album.id, parentWidget());
	albumMenu->popup(mapToGlobal(pos));
}
