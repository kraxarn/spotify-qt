#pragma once

#include "role/tracklistroles.hpp"
#include "lib/spotify/track.hpp"
#include "metatypes.hpp"
#include "lib/vector.hpp"
#include "lib/settings.hpp"
#include "role/tracklistcolumn.hpp"
#include "util/dateutils.hpp"
#include "lib/spotify/api.hpp"
#include "list/tracklistitem.hpp"

#include <QAbstractListModel>
#include <QLocale>
#include <QIcon>

class TrackListModel: public QAbstractListModel
{
Q_OBJECT

public:
	explicit TrackListModel(const lib::settings &settings, QObject *parent);

	void add(const std::vector<lib::spt::track> &tracks);

	void remove(lib::spt::track &track);

	void clear();

	auto rowCount(const QModelIndex &parent) const -> int override;
	auto rowCount() const -> int;

	auto columnCount(const QModelIndex &parent) const -> int override;

	auto data(const QModelIndex &index, int role) const -> QVariant override;
	auto data(const QModelIndex &index) const -> QVariant;

	auto headerData(int section, Qt::Orientation orientation,
		int role) const -> QVariant override;

	auto at(int index) -> const TrackListItem &;

	auto trackIds() const -> std::vector<std::string>;

protected:
	auto roleNames() const -> QHash<int, QByteArray> override;

private:
	const lib::settings &settings;
	std::vector<TrackListItem> tracks;
	QIcon emptyIcon;

	/** Qt::DisplayRole */
	auto displayRole(TrackListColumn column, int row) const -> QString;

	/** Qt::ToolTipRole */
	auto tooltip(TrackListColumn column, int row) const -> QString;
};
