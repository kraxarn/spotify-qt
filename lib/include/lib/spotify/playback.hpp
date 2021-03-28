#pragma once

#include "thirdparty/json.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/device.hpp"
#include "lib/format.hpp"
#include "lib/spotify/context.hpp"
#include "lib/enum/repeatstate.hpp"

namespace lib
{
	namespace spt
	{
		class playback
		{
		public:
			playback() = default;

			/**
			 * Metadata for MPRIS
			 */
			nlohmann::json metadata() const;

			/**
			 * Current volume, fetched from active device
			 */
			int volume() const;

			/**
			 * Context currently playing from
			 */
			lib::spt::context context;

			/**
			 * Repeat mode
			 */
			lib::repeat_state repeat = lib::repeat_state::off;

			/**
			 * Current playing track
			 */
			lib::spt::track item;

			/**
			 * Is currently playing something
			 */
			bool is_playing = false;

			/**
			 * Shuffle tracks
			 */
			bool shuffle = false;

			/**
			 * Progress in current track, in milliseconds
			 */
			int progress_ms = 0;

			/**
			 * Current active device
			 */
			lib::spt::device device;
		};

		/**
		 * json -> playback
		 */
		void from_json(const nlohmann::json &j, playback &p);

		/**
		 * playback -> json
		 */
		void to_json(nlohmann::json &j, const playback &p);
	}
}