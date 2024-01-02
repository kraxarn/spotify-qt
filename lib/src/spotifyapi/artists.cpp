#include "lib/uri.hpp"
#include "lib/spotify/api.hpp"

// Currently unavailable:
// artists

void lib::spt::api::artist(const std::string &id,
	lib::callback<lib::spt::artist> &callback)
{
	get(lib::fmt::format("artists/{}", id), callback);
}

void lib::spt::api::top_tracks(const lib::spt::artist &artist,
	lib::callback<std::vector<lib::spt::track>> &callback)
{
	get(lib::fmt::format("artists/{}/top-tracks?country=from_token",
		artist.id), [callback](const nlohmann::json &json)
	{
		callback(json.at("tracks"));
	});
}

void lib::spt::api::related_artists(const lib::spt::artist &artist,
	lib::callback<std::vector<lib::spt::artist>> &callback)
{
	get(lib::fmt::format("artists/{}/related-artists",
		artist.id), [callback](const nlohmann::json &json)
	{
		callback(json.at("artists"));
	});
}

void lib::spt::api::albums(const spt::artist &artist, const std::vector<album_group> &groups,
	const paged_callback<spt::album> &callback) const
{
	uri uri(to_full_url(fmt::format("artists/{}/albums", artist.id)));

	auto params = uri.get_search_params();
	params.insert({"limit", "50"});

	if (!groups.empty())
	{
		std::vector<std::string> groups_str;
		groups_str.reserve(groups.size());

		for (const auto &group: groups)
		{
			groups_str.push_back(enums<album_group>::to_string(group));
		}

		params.insert({"include_groups", strings::join(groups_str, ",")});
	}

	uri.set_search_params(params);
	request.get_page<spt::album>(uri.get_url(), {}, callback);
}
