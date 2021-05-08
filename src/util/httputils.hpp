#pragma once

#include "lib/spotify/callback.hpp"
#include "lib/cache.hpp"
#include "lib/httpclient.hpp"

#include <string>
#include <QPixmap>

class HttpUtils
{
public:
	/**
	 * Get album from cache or from HTTP
	 */
	static void getAlbum(const lib::http_client &httpClient, const std::string &url,
		lib::cache &cache, lib::callback<QPixmap> &callback);

private:
	HttpUtils() = default;
};
