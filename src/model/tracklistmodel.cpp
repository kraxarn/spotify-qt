#include "tracklistmodel.hpp"

TrackListModel::TrackListModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

void TrackListModel::add(const std::vector<lib::spt::track> &items)
{
	beginInsertRows(QModelIndex(), rowCount(),
		static_cast<int>(rowCount() + tracks.size() - 1));
	lib::vector::append(tracks, items);
	endInsertRows();
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
	if (index.row() < 0 || static_cast<size_t>(index.row()) >= tracks.size())
	{
		return QVariant();
	}

	switch (static_cast<TrackListRole>(role))
	{
		case TrackListRole::Track:
			return QVariant::fromValue(tracks.at(index.row()));

		case TrackListRole::Index:
			return index.row();
	}

	return QVariant();
}

auto TrackListModel::data(const QModelIndex &index) const -> QVariant
{
	return data(index, Qt::DisplayRole);
}

auto TrackListModel::roleNames() const -> QHash<int, QByteArray>
{
	return {
		{static_cast<int>(TrackListRole::Track), "track"},
		{static_cast<int>(TrackListRole::Index), "index"},
	};
}

auto TrackListModel::parent(const QModelIndex &/*child*/) const -> QModelIndex
{
	return {};
}
