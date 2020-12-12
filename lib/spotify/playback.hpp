#pragma once

#include "device.hpp"
#include "format.hpp"
#include "track.hpp"

#include "lib/json.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * Currently playing
		 */
		class Playback
		{
		public:
			Playback() = default;
			explicit Playback(const nlohmann::json &json);

			nlohmann::json metadata() const;
			nlohmann::json toJson() const;

			int volume() const;

			std::string contextType;
			std::string contextUri;
			std::string repeat = "off";
			track item = track();
			bool isPlaying = false;
			bool shuffle = false;
			int progressMs = 0;
			Device device;
		};
	}
}