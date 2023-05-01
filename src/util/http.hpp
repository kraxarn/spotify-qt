#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "lib/httpclient.hpp"
#include "lib/image.hpp"
#include "util/icon.hpp"

#include <string>
#include <QPixmap>

class Http
{
public:
	/**
	 * Get album image from cache or from HTTP
	 * @param url URL to get album from
	 * @param httpClient HTTP client to use
	 * @param cache Cache instance to album cache
	 * @param useDefaultIcon If no cache, call callback first with default icon
	 * @param callback Callback to call one or more times
	 */
	static void getAlbumImage(const std::string &url, const lib::http_client &httpClient,
		lib::cache &cache, bool useDefaultIcon, lib::callback<QPixmap> &callback);

	/**
	 * Get album image from cache or from HTTP, using default icon
	 */
	static void getAlbumImage(const std::string &url, const lib::http_client &httpClient,
		lib::cache &cache, lib::callback<QPixmap> &callback);

	/**
	 * Get album from cache or from HTTP
	 * @param albumId ID of album
	 * @param spotify Spotify API instance
	 * @param cache Cache instance
	 * @param callback Callback called once when loaded
	 */
	static void getAlbum(const std::string &albumId, lib::spt::api &spotify,
		lib::cache &cache, lib::callback<lib::spt::album> &callback);

private:
	Http() = default;

	static auto defaultIcon() -> QPixmap;
};
