#include "view/search/albums.hpp"
#include "mainwindow.hpp"

View::Search::Albums::Albums(lib::spt::api &spotify, lib::cache &cache,
	const lib::http_client &httpClient, QWidget *parent)
	: spotify(spotify),
	cache(cache),
	httpClient(httpClient),
	View::Search::SearchTabTree({"Title", "Artist"}, parent)
{
	QTreeWidget::connect(this, &QTreeWidget::itemClicked,
		this, &View::Search::Albums::onItemClicked);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &View::Search::Albums::onContextMenu);
}

void View::Search::Albums::add(const lib::spt::album &album)
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

	item->setData(0, static_cast<int>(DataRole::AlbumId), id);
	item->setToolTip(0, name);
	item->setToolTip(1, artist);
	addTopLevelItem(item);
}

void View::Search::Albums::onItemClicked(QTreeWidgetItem *item, int /*column*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	const auto albumId = item->data(0, static_cast<int>(DataRole::AlbumId))
		.toString().toStdString();

	if (!mainWindow->loadAlbum(albumId))
	{
		StatusMessage::error(QStringLiteral("Failed to load album"));
	}
}

void View::Search::Albums::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	const auto &albumId = item->data(0, static_cast<int>(DataRole::AlbumId)).toString();
	if (albumId.isEmpty())
	{
		return;
	}

	auto *albumMenu = new Menu::Album(spotify, cache,
		albumId.toStdString(), parentWidget());
	albumMenu->popup(mapToGlobal(pos));
}
