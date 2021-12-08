#pragma once

#include "lib/json.hpp"

namespace lib
{
	namespace setting
	{
		/**
		 * Qt Widgets specific settings
		 */
		using qt = struct qt
		{
			/**
			 * Show system borders and title bar
			 */
			bool system_title_bar = false;

			/**
			 * Use custom font when using custom dark theme
			 */
			bool custom_font = true;

			/**
			 * Mirror buttons in title bar
			 * (requires system_title_bar to be false)
			 */
			bool mirror_title_bar
#ifdef __APPLE__
				= true;
#else
				= false;
#endif
		};

		void to_json(nlohmann::json &j, const qt &q);

		void from_json(const nlohmann::json &j, qt &q);
	}
}
