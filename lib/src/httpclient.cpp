#include "lib/httpclient.hpp"

void lib::http_client::post(const std::string &url, const lib::headers &headers,
	lib::callback<std::string> &callback) const
{
	post(url, std::string(), headers, callback);
}
