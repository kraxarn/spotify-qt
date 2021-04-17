#include "lib/spotify/entity.hpp"

void lib::spt::to_json(nlohmann::json &j, const entity &e)
{
	j = nlohmann::json{
		{"id", e.id},
		{"name", e.name},
	};
}

void lib::spt::from_json(const nlohmann::json &j, entity &e)
{
	if (!j.is_object())
	{
		return;
	}

	if (j.contains("id"))
	{
		j.at("id").get_to(e.id);
	}

	if (j.contains("name"))
	{
		j.at("name").get_to(e.name);
	}
}

auto lib::spt::entity::combine_names(const std::vector<entity> &entities,
	const char *separator) -> std::string
{
	std::vector<std::string> names;
	names.reserve(entities.size());
	for (const auto &entity : entities)
	{
		names.push_back(entity.name);
	}
	return lib::strings::join(names, separator);
}

auto lib::spt::entity::combine_names(const std::vector<entity> &entities) -> std::string
{
	return combine_names(entities, ", ");
}
