#pragma once

#include "lib/spotify/track.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * Playback queue
		 */
		class queue
		{
		public:
			queue() = default;
			virtual ~queue() = default;

			/**
			 * Currently playing track, can be empty
			 */
			lib::spt::track currently_playing;

			/**
			 * All tracks in queue
			 */
			std::vector<lib::spt::track> tracks;
		};

		void from_json(const nlohmann::json &json, queue &queue);
	}
}
