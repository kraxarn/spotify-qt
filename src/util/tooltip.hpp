#pragma once

#include "util/tooltiprow.hpp"
#include "lib/settings.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/album.hpp"
#include "lib/httpclient.hpp"
#include "lib/cache.hpp"

#include <QListWidgetItem>
#include <QTreeWidgetItem>

class Tooltip
{
public:
	Tooltip(lib::settings &settings,
		const lib::http_client &httpClient, lib::cache &cache);

	void set(QListWidgetItem *item, const lib::spt::track &track);
	void set(QTreeWidgetItem *item, const lib::spt::track &track);
	void set(QTreeWidgetItem *item, const lib::spt::track &track, const QPixmap &albumImage);
	void set(QTreeWidgetItem *item, const lib::spt::album &album);

private:
	static constexpr int iconSize = 16;
	static constexpr int albumSize = lib::spt::image::size_small;

	lib::settings &settings;
	const lib::http_client &httpClient;
	lib::cache &cache;

	static auto tooltip(const QPixmap &image, const QList<TooltipRow> &rows) -> QString;

	auto tooltip(const lib::spt::track &track, const QPixmap &albumImage) -> QString;
	auto tooltip(const lib::spt::album &album, const QPixmap &albumImage) -> QString;

	static auto icon(const QIcon &iconData, int size = iconSize) -> QString;
	static auto icon(const QPixmap &pixmap) -> QString;

	auto albumIcon(const QPixmap &albumPixmap) -> QPixmap;
};
