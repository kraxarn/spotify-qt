#pragma once

#include "thirdparty/json.hpp"

#include "lib/spotify/entity.hpp"

#include <string>

namespace lib
{
	namespace spt
	{
		/**
		 * Playback device
		 */
		class device: public entity
		{
		public:
			device() = default;

			/**
			 * Device type, for example computer or speaker
			 */
			std::string type;

			/**
			 * Is current device
			 */
			bool is_active = false;

			/**
			 * Volume from 0-100%
			 */
			int volume_percent = 0;
		};

		/**
		 * device -> json
		 */
		void to_json(nlohmann::json &j, const device &d);

		/**
		 * json -> device
		 */
		void from_json(const nlohmann::json &j, device &d);
	}
}
