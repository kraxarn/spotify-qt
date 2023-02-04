#pragma once

#include "lib/json.hpp"
#include "lib/enum/audioquality.hpp"
#include "lib/enum/devicetype.hpp"

#include <string>

namespace lib
{
	namespace setting
	{
		/**
		 * Spotify client settings
		 */
		using spotify = struct spotify
		{
			/**
			 * Name of backend to use
			 */
			std::string backend;

			/**
			 * Path to Spotify client
			 */
			std::string path;

			/**
			 * Spotify username
			 */
			std::string username;

			/**
			 * Always start Spotify client on application start
			 */
			bool always_start = true;

			/**
			 * Use global config instead of config in settings
			 */
			bool global_config = false;

			/**
			 * Start Spotify client on application start
			 */
			bool start_client = false;

			/**
			 * Disable discovery mode
			 * @note librespot only
			 */
			bool disable_discovery = false;

			/**
			 * Bitrate for Spotify client
			 * @note Required to be normal, high or very_high
			 */
			lib::audio_quality bitrate = lib::audio_quality::very_high;

			/**
			 * Max items allowed to be queued
			 */
			int max_queue = 500;

			/**
			 * Device type for Spotify client
			 */
			lib::device_type device_type = device_type::unknown;

			/**
			 * Volume to start client at from 0-100%
			 */
			int volume = 100;
		};

		void to_json(nlohmann::json &j, const spotify &s);

		void from_json(const nlohmann::json &j, spotify &s);
	}
}
