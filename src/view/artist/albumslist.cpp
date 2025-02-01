#include "view/artist/albumslist.hpp"
#include "util/tooltip.hpp"
#include "mainwindow.hpp"

Artist::AlbumsList::AlbumsList(lib::spt::api &spotify, lib::cache &cache,
	const lib::http_client &httpClient, lib::settings &settings, QWidget *parent)
	: QTreeWidget(parent),
	spotify(spotify),
	cache(cache),
	httpClient(httpClient),
	tooltip(settings, httpClient, cache)
{
	setEnabled(false);
	setColumnCount(2);
	setMouseTracking(true);

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

	QTreeWidget::connect(this, &QTreeWidget::itemEntered,
		this, &Artist::AlbumsList::onItemEntered);

	connect(this, &QTreeWidget::itemExpanded,
		this, &AlbumsList::onItemExtended);

	for (auto i = lib::album_group::album; i < lib::album_group::none;
		i = static_cast<lib::album_group>(static_cast<int>(i) + 1))
	{
		groups[i] = new QTreeWidgetItem(this, {groupToString(i)});
		addTopLevelItem(groups[i]);
	}
}

auto Artist::AlbumsList::getAlbum(QTreeWidgetItem *item) -> lib::spt::album
{
	const auto itemData = item->data(0, static_cast<int>(DataRole::Album));
	return itemData.value<lib::spt::album>();
}

void Artist::AlbumsList::loadAlbums(const lib::spt::page<lib::spt::album> &page)
{
	setEnabled(false);
	addAlbums(page.items);

	if (page.has_next())
	{
		return;
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

void Artist::AlbumsList::addAlbums(const std::vector<lib::spt::album> &albums) const
{
	for (const auto &album: albums)
	{
		const auto releaseDate = DateTime::parseIsoDate(album.release_date);
		// Extra spacing is intentional so year doesn't overlap with scrollbar
		const auto year = releaseDate.toString("yyyy    ");

		const auto albumName = QString::fromStdString(album.name);

		auto *group = groups.at(album.album_group);
		auto *item = new QTreeWidgetItem(group, {
			albumName,
			year.isEmpty() ? QString() : year
		});

		Http::getAlbumImage(album.image, httpClient, cache, [item](const QPixmap &image)
		{
			if (item != nullptr)
			{
				item->setIcon(0, QIcon(image));
			}
		});

		item->setData(0, static_cast<int>(DataRole::Album),
			QVariant::fromValue(album));

		// We don't want to show added "-01" in tooltip
		const auto fullReleaseDate = DateTime::parseIso(album.release_date);
		const auto releaseDateToolTip = fullReleaseDate.isValid()
			? QLocale::system().toString(fullReleaseDate.date(), QLocale::ShortFormat)
			: QString::fromStdString(album.release_date);

		item->setToolTip(1, releaseDateToolTip);

		group->addChild(item);
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
	const auto id = getAlbum(item).id;
	if (id.empty())
	{
		item->setSelected(false);
		return;
	}

	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->loadAlbum(id);
}

void Artist::AlbumsList::onItemDoubleClicked(QTreeWidgetItem *item, int /*column*/)
{
	const auto id = getAlbum(item).id;
	if (id.empty())
	{
		item->setSelected(false);
		return;
	}

	spotify.play_tracks(lib::spt::id_to_uri("album", id),
		[](const std::string &result)
		{
			if (result.empty())
			{
				return;
			}

			StatusMessage::error(QStringLiteral("Failed to start playback: %1")
				.arg(QString::fromStdString(result)));
		});
}

void Artist::AlbumsList::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	auto albumId = getAlbum(item).id;
	if (albumId.empty())
	{
		return;
	}

	auto *albumMenu = new Menu::Album(spotify, cache, albumId, parentWidget());
	albumMenu->popup(mapToGlobal(pos));
}

void Artist::AlbumsList::onItemEntered(QTreeWidgetItem *item, int column)
{
	if (!item->toolTip(0).isEmpty() || column != 0 || item->parent() == nullptr)
	{
		return;
	}

	const auto album = getAlbum(item);
	tooltip.set(item, album, item->icon(0));
}

void Artist::AlbumsList::onItemExtended(const QTreeWidgetItem *item) const
{
	constexpr auto pageSize = 50;
	if (item == nullptr || item->childCount() != pageSize)
	{
		return;
	}

	if (item->text(0) != groupToString(lib::album_group::appears_on))
	{
		return;
	}

	const auto *view = Parent::findWidget<View>(parentWidget());
	if (view == nullptr)
	{
		lib::log::debug("Parent isn't view ({})",
			parent()->metaObject()->className());
		return;
	}

	auto *group = groups.at(lib::album_group::appears_on);
	group->setDisabled(true);

	const auto &artist = view->getArtist();

	spotify.albums(artist, {lib::album_group::appears_on},
		[this, group](const lib::result<lib::spt::page<lib::spt::album>> &result) -> bool
		{
			if (!result.success())
			{
				StatusMessage::error(QStringLiteral("Failed to load appears on albums: %1")
					.arg(QString::fromStdString(result.message())));

				return false;
			}

			const auto &page = result.value();
			if (page.offset == 0)
			{
				group->setDisabled(true);
				while (const auto *child = group->takeChild(0))
				{
					delete child;
				}
			}

			addAlbums(page.items);

			if (page.has_next())
			{
				return true;
			}

			group->setDisabled(false);
			return false;
		});
}
