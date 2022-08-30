#include "lib/spotify/request.hpp"
#include "lib/fmt.hpp"

auto lib::spt::request::to_full_url(const std::string &relative_url) -> std::string
{
	return lib::fmt::format("https://api.spotify.com/v1/{}", relative_url);
}
