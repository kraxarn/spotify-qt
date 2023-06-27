#include "util/tooltip.hpp"
#include "util/icon.hpp"
#include "util/datetime.hpp"
#include "util/image.hpp"
#include "util/http.hpp"

#include <QBuffer>

Tooltip::Tooltip(lib::settings &settings,
	const lib::http_client &httpClient, lib::cache &cache)
	: settings(settings),
	httpClient(httpClient),
	cache(cache)
{
}

void Tooltip::set(QListWidgetItem *item, const lib::spt::track &track)
{
	const auto icon = item->icon().pixmap(albumSize, albumSize);
	item->setToolTip(tooltip(track, icon));
}

void Tooltip::set(QTreeWidgetItem *item, const lib::spt::track &track)
{
	Http::getAlbumImage(track.image_small(), httpClient, cache,
		[this, item, track](const QPixmap &pixmap)
		{
			set(item, track, pixmap);
		});
}

void Tooltip::set(QTreeWidgetItem *item, const lib::spt::track &track, const QPixmap &albumImage)
{
	item->setToolTip(0, tooltip(track, albumImage));
}

void Tooltip::set(QTreeWidgetItem *item, const lib::spt::album &album)
{
	const auto icon = item->icon(0).pixmap(albumSize, albumSize);
	item->setToolTip(0, tooltip(album, icon));
}

auto Tooltip::tooltip(const QPixmap &image, const QList<TooltipRow> &rows) -> QString
{
	QString table("<table>");

	for (qsizetype i = 0; i < rows.size(); i++)
	{
		table.append("<tr>");
		const auto &row = rows.at(i);

		if (i == 0 && !image.isNull())
		{
			table.append(QString(R"(<td rowspan="%1"><img src="%2"/></td>)")
				.arg(rows.size())
				.arg(icon(image)));
		}

		table.append(QString(R"(<td><img src="%1"/></td><td>%2</td>)")
			.arg(icon(row.icon), row.name));

		table.append("</tr>");
	}

	table.append("</table>");
	return table;
}

auto Tooltip::tooltip(const lib::spt::track &track, const QPixmap &albumImage) -> QString
{
	const auto artists = lib::spt::entity::combine_names(track.artists);

	return tooltip(albumIcon(albumImage), {
		{
			Icon::get(QStringLiteral("view-media-track")),
			QString::fromStdString(track.name),
		},
		{
			Icon::get(QStringLiteral("view-media-artist")),
			QString::fromStdString(artists),
		},
		{
			Icon::get(QStringLiteral("view-media-album-cover")),
			QString::fromStdString(track.album.name),
		}
	});
}

auto Tooltip::tooltip(const lib::spt::album &album, const QPixmap &albumImage) -> QString
{
	const auto locale = QLocale::system();
	const auto releaseDate = DateTime::parseIsoDate(album.release_date);

	return tooltip(albumIcon(albumImage), {
		{
			Icon::get(QStringLiteral("view-media-album-cover")),
			QString::fromStdString(album.name),
		},
		{
			Icon::get(QStringLiteral("view-media-artist")),
			QString::fromStdString(album.artist),
		},
		{
			Icon::get(QStringLiteral("view-calendar")),
			locale.toString(releaseDate.date()),
		},
	});
}

auto Tooltip::icon(const QIcon &iconData, int size) -> QString
{
	const auto pixmap = iconData.pixmap(size, size);
	return icon(pixmap);
}

auto Tooltip::icon(const QPixmap &pixmap) -> QString
{
	QByteArray data;
	QBuffer buffer(&data);
	pixmap.save(&buffer, "PNG");

	return QString("data:image/png;base64,%1")
		.arg(QString(data.toBase64()));
}

auto Tooltip::albumIcon(const QPixmap &albumPixmap) -> QPixmap
{
	if (!albumPixmap.isNull())
	{
		const auto shape = settings.qt().album_shape;
		return Image::mask(albumPixmap, shape);
	}

	return Icon::get(QStringLiteral("media-optical-audio"))
		.pixmap(albumSize, albumSize);
}
