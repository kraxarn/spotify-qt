#include "view/artist/albums.hpp"

View::Artist::Albums::Albums(lib::spt::api &spotify, lib::cache &cache,
	const lib::http_client &httpClient, QWidget *parent)
	: cache(cache),
	httpClient(httpClient),
	QTabWidget(parent)
{
	for (auto i = lib::album_group::album; i < lib::album_group::none;
		i = static_cast<lib::album_group>(static_cast<int>(i) + 1))
	{
		lists[i] = new View::Artist::AlbumList(spotify, cache, this);
		addTab(lists[i], toString(i));
	}
}

void View::Artist::Albums::setAlbums(const std::vector<lib::spt::album> &albums)
{
	for (const auto &list : lists)
	{
		list.second->setEnabled(false);
	}

	for (const auto &album : albums)
	{
		const auto releaseDate = DateUtils::fromIso(album.release_date);
		const auto year = releaseDate.toString("yyyy");

		auto *parentTab = lists.at(album.album_group);
		auto *item = new QTreeWidgetItem(parentTab, {
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
		parentTab->insertTopLevelItem(0, item);
	}

	for (const auto &list : lists)
	{
		list.second->setEnabled(true);
	}
}

auto View::Artist::Albums::toString(lib::album_group albumGroup) -> QString
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
