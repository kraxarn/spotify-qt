#pragma once

#include <string>

namespace lib
{
	namespace set
	{
		/**
		 * Spotify client settings
		 */
		typedef struct spotify
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
			 * Save password in system keyring
			 */
			bool keyring_password = false;

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
			 * @note Required to be 96, 160 or 320
			 */
			int bitrate = 320;

			/**
			 * Max items allowed to be queued
			 */
			int max_queue = 500;
		} spotify;
	}
}
