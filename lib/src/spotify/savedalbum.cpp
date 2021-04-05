#include "lib/spotify/savedalbum.hpp"

void lib::spt::to_json(nlohmann::json &j, const saved_album &s)
{
	j = nlohmann::json{
		{"added_at", s.added_at},
		{"album", s.album},
	};
}

void lib::spt::from_json(const nlohmann::json &j, saved_album &s)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("added_at").get_to(s.added_at);
	j.at("album").get_to(s.album);
}
