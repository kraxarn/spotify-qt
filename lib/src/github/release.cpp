#include "lib/github/release.hpp"

void lib::gh::from_json(const nlohmann::json &j, release &r)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("body").get_to(r.body);
}
