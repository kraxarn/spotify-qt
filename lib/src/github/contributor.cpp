#include "lib/github/contributor.hpp"

void lib::gh::from_json(const nlohmann::json &j, contributor &c)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("login").get_to(c.login);
	j.at("avatar_url").get_to(c.avatar_url);
}
