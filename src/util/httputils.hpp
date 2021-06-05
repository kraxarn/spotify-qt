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
	static void getAlbum(const std::string &url, const lib::http_client &httpClient,
		lib::cache &cache, lib::callback<QPixmap> &callback);

private:
	HttpUtils() = default;
};
