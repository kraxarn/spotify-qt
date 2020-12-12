#include "spotify.hpp"

lib::spt::spotify::spotify(lib::Settings &settings)
	: api(settings)
{
}

nlohmann::json lib::spt::spotify::get(const std::string &url)
{
	return api.get(url);
}
