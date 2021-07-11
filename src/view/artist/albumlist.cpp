#include "albumlist.hpp"
#include "mainwindow.hpp"

View::Artist::AlbumList::AlbumList(lib::spt::api &spotify, lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	cache(cache),
	QTreeWidget(parent)
{
	setEnabled(false);
	setColumnCount(2);
	setRootIsDecorated(false);

	header()->hide();
	header()->resizeSection(0, 235);
	header()->resizeSection(1, 1);

	QTreeWidget::connect(this, &QTreeWidget::itemClicked,
		this, &View::Artist::AlbumList::onItemClicked);

	QTreeWidget::connect(this, &QTreeWidget::itemDoubleClicked,
		this, &View::Artist::AlbumList::onItemDoubleClicked);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &View::Artist::AlbumList::onContextMenu);
}

auto View::Artist::AlbumList::albumId(QTreeWidgetItem *item) -> std::string
{
	return item->data(0, RoleAlbumId).toString().toStdString();
}

void View::Artist::AlbumList::onItemClicked(QTreeWidgetItem *item, int /*column*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	if (!mainWindow->loadAlbum(albumId(item)))
	{
		mainWindow->setStatus(QString("Failed to load album"), true);
	}
}

void View::Artist::AlbumList::onItemDoubleClicked(QTreeWidgetItem *item, int /*column*/)
{
	spotify.play_tracks(lib::spt::api::to_uri("album", albumId(item)),
		[this](const std::string &result)
		{
			if (result.empty())
			{
				return;
			}

			auto *mainWindow = MainWindow::find(this->parentWidget());
			mainWindow->status(lib::fmt::format("Failed to start playback: {}",
				result), true);
		});
}

void View::Artist::AlbumList::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	auto albumId = item->data(0, RoleAlbumId).toString();
	if (albumId.isEmpty())
	{
		return;
	}

	auto *albumMenu = new AlbumMenu(spotify, cache, albumId.toStdString(),
		parentWidget());
	albumMenu->popup(mapToGlobal(pos));
}
