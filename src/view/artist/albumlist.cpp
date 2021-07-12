#include "albumlist.hpp"
#include "mainwindow.hpp"

View::Artist::AlbumList::AlbumList(lib::spt::api &spotify, lib::cache &cache,
	const lib::http_client &httpClient, QWidget *parent)
	: spotify(spotify),
	cache(cache),
	httpClient(httpClient),
	QTreeWidget(parent)
{
	setEnabled(false);
	setColumnCount(2);

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

	for (auto i = lib::album_group::album; i < lib::album_group::none;
		i = static_cast<lib::album_group>(static_cast<int>(i) + 1))
	{
		groups[i] = new QTreeWidgetItem(this, {
			groupToString(i),
			QString(),
		});
		addTopLevelItem(groups[i]);
	}
}

auto View::Artist::AlbumList::albumId(QTreeWidgetItem *item) -> std::string
{
	return item->data(0, RoleAlbumId).toString().toStdString();
}

void View::Artist::AlbumList::setAlbums(const std::vector<lib::spt::album> &albums)
{
	setEnabled(false);

	for (const auto &album : albums)
	{
		const auto releaseDate = DateUtils::fromIso(album.release_date);
		// Extra spacing is intentional so year doesn't overlap with scrollbar
		const auto year = releaseDate.toString("yyyy    ");

		auto *group = groups.at(album.album_group);
		auto *item = new QTreeWidgetItem(group, {
			QString::fromStdString(album.name),
			year.isEmpty() ? QString() : year
		});

		HttpUtils::getAlbum(album.image, httpClient, cache, [item](const QPixmap &image)
		{
			if (item != nullptr)
			{
				item->setIcon(0, QIcon(image));
			}
		});

		item->setData(0, RoleAlbumId, QString::fromStdString(album.id));
		item->setToolTip(1, QLocale::system()
			.toString(releaseDate.date(), QLocale::FormatType::ShortFormat));
		group->addChild(item);
	}

	setEnabled(true);

	// Expand albums by default
	groups.at(lib::album_group::album)->setExpanded(true);
}

auto View::Artist::AlbumList::groupToString(lib::album_group albumGroup) -> QString
{
	switch (albumGroup)
	{
		case lib::album_group::album:
			return QString("Albums");

		case lib::album_group::single:
			return QString("Singles");

		case lib::album_group::compilation:
			return QString("Compilations");

		case lib::album_group::appears_on:
			return QString("Appears on");

		case lib::album_group::none:
			return QString("Other");

		default:
			return QString();
	}
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
