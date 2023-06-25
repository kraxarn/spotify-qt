#include "util/tooltip.hpp"
#include "util/icon.hpp"

#include <QBuffer>

void Tooltip::set(QListWidgetItem *item, const lib::spt::track &track)
{
	item->setToolTip(tooltip(track, {}));
}

void Tooltip::set(QListWidgetItem *item, const lib::spt::track &track, const QPixmap &albumImage)
{
	item->setToolTip(tooltip(track, albumImage));
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
	const auto albumPixmap = albumImage.isNull()
		? Icon::get(QStringLiteral("media-optical-audio"))
			.pixmap(albumSize, albumSize)
		: albumImage;

	const auto artists = lib::spt::entity::combine_names(track.artists);

	return tooltip(albumPixmap, {
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
