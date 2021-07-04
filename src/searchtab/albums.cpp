#include "searchtab/albums.hpp"
#include "mainwindow.hpp"

SearchTab::Albums::Albums(lib::spt::api &spotify, lib::cache &cache,
	const lib::http_client &httpClient, QWidget *parent)
	: spotify(spotify),
	cache(cache),
	httpClient(httpClient),
	SearchTab::SearchTabTree({"Title", "Artist"}, parent)
{
	QTreeWidget::connect(this, &QTreeWidget::itemClicked,
		this, &SearchTab::Albums::onItemClicked);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &SearchTab::Albums::onContextMenu);
}

void SearchTab::Albums::add(const lib::spt::album &album)
{
	auto id = QString::fromStdString(album.id);
	auto name = QString::fromStdString(album.name);
	auto artist = QString::fromStdString(album.artist);

	auto *item = new QTreeWidgetItem({
		name, artist
	});

	HttpUtils::getAlbum(album.image, httpClient, cache, [item](const QPixmap &image)
	{
		if (item != nullptr)
		{
			item->setIcon(0, image);
		}
	});

	item->setData(0, RoleAlbumId, id);
	item->setToolTip(0, name);
	item->setToolTip(1, artist);
	addTopLevelItem(item);
}

void SearchTab::Albums::onItemClicked(QTreeWidgetItem *item, int /*column*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	const auto albumId = item->data(0, RoleAlbumId).toString().toStdString();

	if (!mainWindow->loadAlbum(albumId))
	{
		mainWindow->setStatus(QString("Failed to load album"), true);
	}
}

void SearchTab::Albums::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	const auto &albumId = item->data(0, RoleAlbumId).toString();
	if (albumId.isEmpty())
	{
		return;
	}

	auto *albumMenu = new AlbumMenu(spotify, cache, albumId.toStdString(), parentWidget());
	albumMenu->popup(mapToGlobal(pos));
}
