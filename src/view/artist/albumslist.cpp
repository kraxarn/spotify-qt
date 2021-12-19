#include "view/artist/albumslist.hpp"
#include "mainwindow.hpp"

Artist::AlbumsList::AlbumsList(lib::spt::api &spotify, lib::cache &cache,
	const lib::http_client &httpClient, QWidget *parent)
	: QTreeWidget(parent),
	spotify(spotify),
	cache(cache),
	httpClient(httpClient)
{
	setEnabled(false);
	setColumnCount(2);

	header()->hide();
	header()->setStretchLastSection(false);
	header()->setSectionResizeMode(0, QHeaderView::Stretch);
	header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

	QTreeWidget::connect(this, &QTreeWidget::itemClicked,
		this, &Artist::AlbumsList::onItemClicked);

	QTreeWidget::connect(this, &QTreeWidget::itemDoubleClicked,
		this, &Artist::AlbumsList::onItemDoubleClicked);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &Artist::AlbumsList::onContextMenu);

	for (auto i = lib::album_group::album; i < lib::album_group::none;
		i = static_cast<lib::album_group>(static_cast<int>(i) + 1))
	{
		groups[i] = new QTreeWidgetItem(this, {groupToString(i)});
		addTopLevelItem(groups[i]);
	}
}

auto Artist::AlbumsList::albumId(QTreeWidgetItem *item) -> std::string
{
	return item->data(0, static_cast<int>(DataRole::AlbumId))
		.toString().toStdString();
}

void Artist::AlbumsList::setAlbums(const std::vector<lib::spt::album> &albums)
{
	setEnabled(false);

	for (const auto &album: albums)
	{
		const auto releaseDate = DateUtils::fromIso(album.release_date);
		// Extra spacing is intentional so year doesn't overlap with scrollbar
		const auto year = releaseDate.toString("yyyy    ");

		const auto albumName = QString::fromStdString(album.name);

		auto *group = groups.at(album.album_group);
		auto *item = new QTreeWidgetItem(group, {
			albumName, year.isEmpty() ? QString() : year
		});

		HttpUtils::getAlbum(album.image, httpClient, cache, [item](const QPixmap &image)
		{
			if (item != nullptr)
			{
				item->setIcon(0, QIcon(image));
			}
		});

		item->setData(0, static_cast<int>(DataRole::AlbumId),
			QString::fromStdString(album.id));

		item->setToolTip(0, albumName);
		item->setToolTip(1, QLocale::system()
			.toString(releaseDate.date(), QLocale::FormatType::ShortFormat));

		group->addChild(item);
	}

	setEnabled(true);

	// Expand first group with items
	for (const auto &group: groups)
	{
		if (group.second->childCount() > 0)
		{
			group.second->setExpanded(true);
			break;
		}
	}
}

auto Artist::AlbumsList::groupToString(lib::album_group albumGroup) -> QString
{
	switch (albumGroup)
	{
		case lib::album_group::album:
			return QStringLiteral("Albums");

		case lib::album_group::single:
			return QStringLiteral("Singles");

		case lib::album_group::compilation:
			return QStringLiteral("Compilations");

		case lib::album_group::appears_on:
			return QStringLiteral("Appears On");

		case lib::album_group::none:
			return QStringLiteral("Other");

		default:
			return {};
	}
}

void Artist::AlbumsList::onItemClicked(QTreeWidgetItem *item, int /*column*/)
{
	const auto id = albumId(item);
	if (id.empty())
	{
		item->setSelected(false);
		return;
	}

	auto *mainWindow = MainWindow::find(parentWidget());
	if (!mainWindow->loadAlbum(id))
	{
		StatusMessage::error(QStringLiteral("Failed to load album"));
	}
}

void Artist::AlbumsList::onItemDoubleClicked(QTreeWidgetItem *item, int /*column*/)
{
	const auto id = albumId(item);
	if (id.empty())
	{
		item->setSelected(false);
		return;
	}

	spotify.play_tracks(lib::spt::api::to_uri("album", id),
		[](const std::string &result)
		{
			if (result.empty())
			{
				return;
			}

			StatusMessage::error(QString("Failed to start playback: %1")
				.arg(QString::fromStdString(result)));
		});
}

void Artist::AlbumsList::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	auto albumId = item->data(0, static_cast<int>(DataRole::AlbumId)).toString();
	if (albumId.isEmpty())
	{
		return;
	}

	auto *albumMenu = new Menu::Album(spotify, cache, albumId.toStdString(),
		parentWidget());
	albumMenu->popup(mapToGlobal(pos));
}
