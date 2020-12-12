#pragma once

#include "lib/base64.h"
#include "lib/httplib.h"
#include "lib/json.hpp"
#include "settings.hpp"
#include "spotify/album.hpp"
#include "spotify/artist.hpp"
#include "spotify/core/result.hpp"
#include "spotify/track.hpp"

namespace lib
{
	namespace spt
	{
		namespace api
		{
			/**
			 * Core functionality of the Spotify API, basically an API wrapper with
			 * paging and automatic refreshing
			 */
			class api
			{
			public:
				/**
				 * Construct a new core object
				 * @param settings Settings for getting and updating authentication details
				 */
				explicit api(Settings &settings);

				//region generic

				/**
				 * GET request
				 * @param url URL to get
				 * @return JSON response
				 */
				nlohmann::json get(const std::string &url);

				//endregion

				//region artist

				/**
				 * Get most popular tracks for artist
				 * @param artist Artist
				 * @return Top tracks
				 */
				std::vector<track> top_tracks(const artist &artist);

				/**
				 * Get all albums by specified artist
				 * @param artist Artist
				 * @return Albums
				 */
				std::vector<Album> albums(const artist &artist);

				/**
				 * Get other artists related to the specified one
				 * @param artist Artist
				 * @return Artists
				 */
				std::vector<artist> related_artists(const artist &artist);

				//endregion

			private:
				/**
				 * Time of last authentication, used for checking if we should refresh
				 */
				std::chrono::system_clock::time_point last_auth;

				/**
				 * Successful refresh has been made at initialization
				 */
				bool refresh_valid = false;

				/**
				 * Local settings instance with api keys
				 */
				Settings &settings;

				/**
				 * HTTP client used for requests
				 */
				httplib::Client http_client;

				/**
				 * Refresh access token
				 * @return If successful
				 */
				core::result refresh();

				/**
				 * Get all items to a vector by using the default constructor
				 * @tparam Result Type of item to collect
				 * @param array JSON array with items
				 * @return Vector with collected items
				 */
				template<typename Result>
				std::vector<Result> get_to_vector(const nlohmann::json &array)
				{
					std::vector<Result> items;
					for (auto &item : array.items())
						items.push_back(Result(item.value()));
					return items;
				}
			};
		}
	}
}