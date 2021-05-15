#include "lib/spotify/trackinfo.hpp"

void lib::spt::to_json(nlohmann::json &j, const lib::spt::track_info &l)
{
	j = nlohmann::json{
		{"lyrics", l.lyrics},
	};
}

void lib::spt::from_json(const nlohmann::json &j, lib::spt::track_info &l)
{
	if (!j.is_object())
	{
		return;
	}

	const auto &lyrics = j.at("lyrics");
	if (!lyrics.is_null() && lyrics.is_string())
	{
		lyrics.get_to(l.lyrics);
	}
}

auto lib::spt::track_info::is_valid() const -> bool
{
	return !lyrics.empty();
}
