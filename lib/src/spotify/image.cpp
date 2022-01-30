#include "lib/spotify/image.hpp"

void lib::spt::from_json(const nlohmann::json &j, image &i)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("url").get_to(i.url);
	j.at("height").get_to(i.height);
	j.at("width").get_to(i.width);
}

void lib::spt::to_json(nlohmann::json &j, const image &i)
{
	j = nlohmann::json{
		{"url", i.url},
		{"height", i.height},
		{"width", i.width},
	};
}

auto lib::spt::image::size() const -> lib::image_size
{
	if (is_size(image_size::small))
	{
		return lib::image_size::small;
	}

	if (is_size(image_size::medium))
	{
		return image_size::medium;
	}

	if (is_size(image_size::large))
	{
		return image_size::large;
	}

	return image_size::unknown;
}

auto lib::spt::image::is_size(lib::image_size image_size) const -> bool
{
	const auto size = static_cast<int>(image_size);
	return height == size || width == size;
}
