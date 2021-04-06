#include "lib/httpclient.hpp"

lib::http_client::http_client(lib::settings &settings)
	: settings(settings)
{
}

auto lib::http_client::authorization_header() const -> std::string
{
	return lib::fmt::format("Bearer {}", settings.account.access_token);
}
