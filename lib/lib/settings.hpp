#pragma once

#include "enum/palette.hpp"
#include "enum/playlistorder.hpp"
#include "enum/spotifycontext.hpp"
#include "log/log.hpp"
#include "paths/paths.hpp"
#include "thirdparty/json.hpp"
#include "thirdparty/filesystem.hpp"
#include "settings/general.hpp"
#include "settings/account.hpp"
#include "settings/spotify.hpp"

#include <mutex>

namespace lib
{
	/**
	 * Application settings
	 */
	class settings
	{
	public:
		/**
		 * Create new instance and attempt to load from file
		 */
		explicit settings(const paths &paths);

		/**
		 * Format settings in settings
		 * @return JSON
		 */
		nlohmann::json to_json() const;

		/**
		 * Load settings from json
		 * @param json JSON
		 */
		void from_json(const nlohmann::json &json);

		/**
		 * Save settings to file
		 */
		void save();

		/**
		 * Load settings from file
		 */
		void load();

		/**
		 * Account settings
		 */
		set::account account;

		/**
		 * General settings
		 */
		set::general general;

		/**
		 * Spotify settings
		 */
		set::spotify spotify;

		/**
		 * Reset client_id and client_secret properties
		 */
		void remove_client();

		/**
		 * Reset access_token and refresh_token properties
		 */
		void remove_tokens();

		/**
		 * Get if dark theme should be used
		 * @return Use dark theme
		 */
		bool get_dark_theme() const;

		/**
		 * Set if dark theme should be used
		 * @param value Dark theme should be used
		 */
		void set_dark_theme(bool value);

		/**
		 * Get config file name
		 * @return File name with path
		 */
		std::string file_name() const;

		/**
		 * Get path to config file name
		 * @return Path
		 */
		std::string file_path() const;

		/**
		 * Validate settings to make sure they are valid
		 * @return Errors
		 */
		std::map<std::string, std::vector<std::string>> validate() const;

	private:
		/**
		 * paths implementation
		 */
		const paths &path;

		/**
		 * Mutex for making sure we don't write to settings from multiple
		 * threads
		 */
		std::mutex mutex;

		/**
		 * Set value from JSON in a "safe" way
		 * @param json JSON object to try and find value from
		 * @param key Key of value in root of JSON
		 * @param value Value to write to
		 */
		template<typename T>
		void setValue(const nlohmann::json &json, const std::string &key,
			T &value)
		{
			if (!json.contains(key))
				return;

			try
			{
				json.at(key).get_to(value);
			}
			catch (const nlohmann::json::exception &e)
			{
				log::error("Failed to parse settings for key: {} ({})",
					key, e.what());
			}
		}
	};
}