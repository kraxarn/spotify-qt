#include "lib/spotify/api.hpp"

#include <string>

void lib::spt::api::album(const std::string &id, lib::callback<lib::spt::album> &callback)
{
	get(lib::fmt::format("albums/{}", id), callback);
}

void lib::spt::api::album_tracks(const spt::album &album, const paged_callback<spt::track> &callback) const
{
	const auto albumName = album.name;
	const auto url = fmt::format("albums/{}/tracks?limit=50", album.id);

	request.get_page<spt::track>(url, {},
		[callback, albumName](const result<page<spt::track>> &result) -> bool
		{
			if (!result.success())
			{
				return callback(result);
			}

			spt::page<spt::track> page = result.value();
			for (auto &item: page.items)
			{
				item.album.name = albumName;
			}

			return callback(lib::result<spt::page<spt::track>>::ok(page));
		});
}
