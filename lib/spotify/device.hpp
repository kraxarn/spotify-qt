#pragma once

#include "lib/json.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * Spotify playback device
		 */
		class Device
		{
		public:
			/**
			 * Default empty constructor, don't use explicitly
			 */
			Device() = default;

			/**
			 * Construct device from json
			 * @param json json data
			 */
			explicit Device(const nlohmann::json &json);

			/**
			 * ID of device, when selecting for example
			 */
			std::string id;

			/**
			 * Name of device
			 */
			std::string name;

			/**
			 * Type of device, see
			 * https://developer.spotify.com/documentation/web-api/reference/player/get-a-users-available-devices/#device-types
			 * for all possible types
			 */
			std::string type;

			/**
			 * Currently active device
			 */
			bool isActive = false;

			/**
			 * Current volume from 0-100
			 */
			int volumePercent = 0;
		};
	}
}