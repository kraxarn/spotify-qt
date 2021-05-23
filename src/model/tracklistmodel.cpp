#include "tracklistmodel.hpp"

TrackListModel::TrackListModel(const lib::settings &settings, QObject *parent)
	: settings(settings),
	QAbstractListModel(parent)
{
	constexpr int emptyPixmapSize = 64;

	// Empty icon used as replacement for play icon
	QPixmap emptyPixmap(emptyPixmapSize, emptyPixmapSize);
	emptyPixmap.fill(Qt::transparent);
	emptyIcon = QIcon(emptyPixmap);
}

void TrackListModel::add(const std::vector<lib::spt::track> &items)
{
	beginInsertRows(QModelIndex(), rowCount(),
		static_cast<int>(rowCount() + tracks.size() - 1));

	auto i = rowCount();
	for (const auto &item : items)
	{
		tracks.emplace_back(item, i++);
	}

	endInsertRows();
}

void TrackListModel::remove(lib::spt::track &item)
{
	std::vector<TrackListItem>::iterator iter;
	for (iter = tracks.begin(); iter != tracks.end(); iter++)
	{
		if (*iter == item)
		{
			break;
		}
	}

	if (iter == tracks.end())
	{
		return;
	}
	auto index = static_cast<int>(std::distance(tracks.begin(), iter));
	beginRemoveRows(QModelIndex(), index, index);
	tracks.erase(iter);
	endRemoveRows();
}

void TrackListModel::clear()
{
	beginRemoveRows(QModelIndex(), 0,
		static_cast<int>(tracks.size() - 1));
	tracks.clear();
	endRemoveRows();
}

auto TrackListModel::rowCount(const QModelIndex &/*parent*/) const -> int
{
	return static_cast<int>(tracks.size());
}

auto TrackListModel::rowCount() const -> int
{
	return rowCount(QModelIndex());
}

auto TrackListModel::columnCount(const QModelIndex &/*parent*/) const -> int
{
	constexpr int columnCount = 6;
	return columnCount;
}

auto TrackListModel::data(const QModelIndex &index, int role) const -> QVariant
{
	auto row = index.row();
	auto col = static_cast<TrackListColumn>(index.column());

	if (row < 0 || row >= tracks.size())
	{
		return QVariant();
	}

	const auto &trackItem = tracks.at(row);

	switch (role)
	{
		// Text to display
		case Qt::DisplayRole:
			return displayRole(col, row);

			// Icon to show
		case Qt::DecorationRole:
			return emptyIcon;

			// Tooltip to show on hover
		case Qt::ToolTipRole:
			return tooltip(col, row);

		case TrackRoleTrack:
			return QVariant::fromValue(trackItem.getTrack());

		case TrackRoleIndex:
			return trackItem.getIndex();

		default:
			return QVariant();
	}
}

auto TrackListModel::data(const QModelIndex &index) const -> QVariant
{
	return data(index, Qt::DisplayRole);
}

auto TrackListModel::roleNames() const -> QHash<int, QByteArray>
{
	return {
		{TrackRoleTrack, "track"},
		{TrackRoleIndex, "index"},
	};
}

auto TrackListModel::headerData(int section, Qt::Orientation orientation,
	int role) const -> QVariant
{
	if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
	{
		return QVariant();
	}

	switch (static_cast<TrackListColumn>(section))
	{
		case TrackListColumn::Number:
			return settings.general.track_numbers == lib::context_all
				? QString("#")
				: QString();

		case TrackListColumn::Name:
			return QString("Title");

		case TrackListColumn::Artists:
			return QString("Artist");

		case TrackListColumn::Album:
			return QString("Album");

		case TrackListColumn::Duration:
			return QString("Length");

		case TrackListColumn::AddedAt:
			return QString("Added");

		default:
			return QVariant();
	}
}

auto TrackListModel::at(int index) -> const TrackListItem &
{
	return tracks.at(index);
}

auto TrackListModel::displayRole(TrackListColumn column, int row) const -> QString
{
	if (column == TrackListColumn::Number)
	{
		// TODO: Slow (needs to be done for each row)?
		auto fieldWidth = static_cast<int>(std::to_string(tracks.size()).size());
		return settings.general.track_numbers == lib::context_all
			? QString("%1").arg(row + 1, fieldWidth)
			: QString();
	}

	const auto &track = tracks.at(row).getTrack();

	if (column == TrackListColumn::Name)
	{
		return QString::fromStdString(track.name);
	}

	if (column == TrackListColumn::Artists)
	{
		auto names = lib::spt::entity::combine_names(track.artists);
		return QString::fromStdString(names);
	}

	if (column == TrackListColumn::Album)
	{
		return QString::fromStdString(track.album.name);
	}

	if (column == TrackListColumn::Duration)
	{
		auto time = lib::fmt::time(track.duration);
		return QString::fromStdString(time);
	}

	if (column == TrackListColumn::AddedAt)
	{
		if (track.added_at.empty())
		{
			return QString();
		}

		if (settings.general.relative_added)
		{
			return DateUtils::toRelative(track.added_at);
		}

		auto date = DateUtils::fromIso(track.added_at).date();
		return QLocale::system().toString(date, QLocale::ShortFormat);
	}

	return QString();
}

auto TrackListModel::tooltip(TrackListColumn column, int row) const -> QString
{
	if (column == TrackListColumn::Number)
	{
		return QString();
	}

	auto trackItem = tracks.at(row);
	const auto &track = trackItem.getTrack();

	if (column == TrackListColumn::Name)
	{
		if (track.is_local)
		{
			return QString("Local track");
		}
		if (!track.is_playable)
		{
			return QString("Unavailable");
		}
		return QString::fromStdString(track.name);
	}

	if (column == TrackListColumn::Artists)
	{
		auto names = lib::spt::entity::combine_names(track.artists, "\n");
		return QString::fromStdString(names);
	}

	if (column == TrackListColumn::Album)
	{
		return QString::fromStdString(track.album.name);
	}

	if (column == TrackListColumn::Duration)
	{
		auto time = lib::fmt::time_min_sec(track.duration);
		return QString("%1m %2s (%3s total)")
			.arg(time.first, time.second)
			.arg(track.duration / 1000);
	}

	if (column == TrackListColumn::AddedAt)
	{
		auto locale = QLocale::system();
		return locale.toString(trackItem.addedAt());
	}

	return QString();
}

auto TrackListModel::trackIds() const -> std::vector<std::string>
{
	std::vector<std::string> items;
	items.reserve(tracks.size());

	for (const auto &trackItem : tracks)
	{
		const auto &track = trackItem.getTrack();

		if (!track.is_valid())
		{
			continue;
		}
		items.push_back(lib::spt::api::to_uri("track", track.id));
	}

	return items;
}
