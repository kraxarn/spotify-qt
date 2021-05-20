#pragma once

#include "role/tracklistroles.hpp"
#include "lib/spotify/track.hpp"
#include "metatypes.hpp"
#include "lib/vector.hpp"

#include <QAbstractListModel>

class TrackListModel: public QAbstractListModel
{
Q_OBJECT

public:
	explicit TrackListModel(QObject *parent);

	void add(const std::vector<lib::spt::track> &tracks);

	void clear();

	auto rowCount(const QModelIndex &parent) const -> int override;
	auto rowCount() const -> int;

	auto columnCount(const QModelIndex &parent) const -> int override;

	auto data(const QModelIndex &index, int role) const -> QVariant override;
	auto data(const QModelIndex &index) const -> QVariant;

	auto parent(const QModelIndex &child) const -> QModelIndex override;

protected:
	auto roleNames() const -> QHash<int, QByteArray> override;

private:
	std::vector<lib::spt::track> tracks;
};
