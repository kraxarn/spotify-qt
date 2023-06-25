#pragma once

#include "util/tooltiprow.hpp"
#include "lib/settings.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/album.hpp"

#include <QListWidgetItem>
#include <QTreeWidgetItem>

class Tooltip
{
public:
	Tooltip(lib::settings &settings);

	void set(QListWidgetItem *item, const lib::spt::track &track);
	void set(QTreeWidgetItem *item, const lib::spt::track &track);
	void set(QTreeWidgetItem *item, const lib::spt::album &album);

private:
	static constexpr int iconSize = 16;
	static constexpr int albumSize = 64;

	lib::settings &settings;

	static auto tooltip(const QPixmap &image, const QList<TooltipRow> &rows) -> QString;

	auto tooltip(const lib::spt::track &track, const QPixmap &albumImage) -> QString;
	auto tooltip(const lib::spt::album &album, const QPixmap &albumImage) -> QString;

	static auto icon(const QIcon &iconData, int size = iconSize) -> QString;
	static auto icon(const QPixmap &pixmap) -> QString;

	auto albumIcon(const QPixmap &albumPixmap) -> QPixmap;
};
