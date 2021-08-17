#include "lib/spotify/device.hpp"

void lib::spt::to_json(nlohmann::json &j, const device &d)
{
	j = nlohmann::json{
		{"id", d.id},
		{"name", d.name},
		{"type", d.type},
		{"is_active", d.is_active},
		{"volume_percent", d.volume_percent},
	};
}

void lib::spt::from_json(const nlohmann::json &j, device &d)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("id").get_to(d.id);
	j.at("name").get_to(d.name);
	j.at("type").get_to(d.type);
	j.at("is_active").get_to(d.is_active);
	j.at("volume_percent").get_to(d.volume_percent);
}
