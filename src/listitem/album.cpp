#include "listitem/album.hpp"
#include "util/datetime.hpp"

ListItem::Album::Album(const lib::spt::album &album, QTreeWidgetItem *parent)
	: QTreeWidgetItem(parent)
{
	const auto albumName = QString::fromStdString(album.name);
	setText(static_cast<int>(Column::Name), albumName);

	// Extra spacing is intentional so year doesn't overlap with the scrollbar
	const auto releaseDate = DateTime::parseIsoDate(album.release_date);
	const auto year = releaseDate.toString(QStringLiteral("yyyy    "));
	setText(static_cast<int>(Column::Year), year.isEmpty() ? QString() : year);
	setData(static_cast<int>(Column::Year), 0x100, releaseDate);
}

auto ListItem::Album::operator<(const QTreeWidgetItem &item) const -> bool
{
	const auto column = static_cast<Column>(treeWidget()->sortColumn());

	if (column == Column::Year)
	{
		const auto date1 = data(static_cast<int>(Column::Year), 0x100).toDateTime();
		const auto date2 = item.data(static_cast<int>(Column::Year), 0x100).toDateTime();
		return date1 < date2;
	}

	const auto text1 = text(static_cast<int>(column));
	const auto text2 = item.text(static_cast<int>(column));
	return text1.compare(text2, Qt::CaseInsensitive) < 0;
}
