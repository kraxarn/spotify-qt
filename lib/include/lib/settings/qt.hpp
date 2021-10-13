#pragma once

#include "thirdparty/json.hpp"

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
		};

		void to_json(nlohmann::json &j, const qt &q);

		void from_json(const nlohmann::json &j, qt &q);
	}
}
