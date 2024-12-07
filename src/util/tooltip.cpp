#include "util/tooltip.hpp"
#include "util/icon.hpp"
#include "util/datetime.hpp"
#include "util/image.hpp"
#include "util/http.hpp"

#include <QBuffer>
#include <QTextDocument>

Tooltip::Tooltip(lib::settings &settings,
	const lib::http_client &httpClient, lib::cache &cache)
	: settings(settings),
	httpClient(httpClient),
	cache(cache)
{
}

void Tooltip::set(QListWidgetItem *item, const lib::spt::track &track, const QIcon &albumIcon)
{
	const auto icon = albumIcon.pixmap(albumSize, albumSize);
	item->setToolTip(tooltip(track, icon));
}

void Tooltip::set(QTreeWidgetItem *item, const lib::spt::track &track)
{
	Http::getAlbumImage(track.image_small(), httpClient, cache,
		[this, item, track](const QPixmap &pixmap)
		{
			item->setToolTip(0, tooltip(track, pixmap));
		});
}

void Tooltip::set(QTreeWidgetItem *item, const lib::spt::album &album)
{
	Http::getAlbumImage(album.image, httpClient, cache,
		[this, item, album](const QPixmap &pixmap)
		{
			item->setToolTip(0, tooltip(album, pixmap));
		});
}

void Tooltip::set(QTreeWidgetItem *item, const lib::spt::album &album, const QIcon &albumIcon)
{
	const auto icon = albumIcon.pixmap(albumSize, albumSize);
	item->setToolTip(0, tooltip(album, icon));
}

void Tooltip::set(QListWidgetItem *item, const lib::spt::playlist &playlist)
{
	item->setToolTip(tooltip(playlist, {}));

	if (playlist.image.empty())
	{
		return;
	}

	// TODO: Cache this
	httpClient.get(playlist.image, lib::headers(), [this, item, playlist](const std::string &str)
	{
		auto data = QByteArray::fromStdString(str);
		if (!lib::image::is_jpeg(std::vector<unsigned char>(data.begin(), data.end())))
		{
			return;
		}

		QPixmap img;
		img.loadFromData(data, "jpeg");
		const auto scaled = img.scaled(albumSize, albumSize,
			Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

		item->setToolTip(tooltip(playlist, scaled));
	});
}

auto Tooltip::tooltip(const QPixmap &image, const QList<TooltipRow> &rows) -> QString
{
	QString table("<table>");

	for (long long i = 0; i < rows.size(); i++)
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

auto Tooltip::tooltip(const lib::spt::playlist &playlist, const QPixmap &image) -> QString
{
	QList<TooltipRow> rows{
		{
			Icon::get(QStringLiteral("view-media-playlist")),
			QString::fromStdString(playlist.name),
		},
		{
			Icon::get(QStringLiteral("view-media-artist")),
			QString::fromStdString(playlist.owner_name),
		},
	};

	if (!playlist.description.empty())
	{
		QTextDocument document;
		document.setHtml(QString::fromStdString(playlist.description));

		rows.append({
			Icon::get(QStringLiteral("description")),
			document.toPlainText(),
		});
	}

	return tooltip(playlistIcon(image), rows);
}

auto Tooltip::icon(const QIcon &iconData) -> QString
{
	const QSize size(iconSize, iconSize);
	const auto pixmap = iconData.pixmap(size);
	return icon(pixmap);
}

auto Tooltip::icon(const QPixmap &pixmap) -> QString
{
	QByteArray data;
	QBuffer buffer(&data);
	pixmap.save(&buffer, "PNG");

	return QStringLiteral("data:image/png;base64,%1")
		.arg(QString(data.toBase64()));
}

auto Tooltip::icon(const QPixmap &pixmap, const QString &fallback) -> QPixmap
{
	if (!pixmap.isNull())
	{
		const auto shape = settings.qt().album_shape;
		return Image::mask(pixmap, shape);
	}

	return Icon::get(fallback)
		.pixmap(albumSize, albumSize);
}

auto Tooltip::albumIcon(const QPixmap &albumPixmap) -> QPixmap
{
	return icon(albumPixmap, QStringLiteral("media-optical-audio"));
}

auto Tooltip::playlistIcon(const QPixmap &albumPixmap) -> QPixmap
{
	return icon(albumPixmap, QStringLiteral("view-media-playlist"));
}
