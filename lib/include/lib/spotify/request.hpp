#pragma once

#include <string>

namespace lib
{
	namespace spt
	{
		class request
		{
		public:

		private:
			/**
			 * Get full API url from relative URL
			 */
			static auto to_full_url(const std::string &relative_url) -> std::string;

			// Until all requests are moved to here
			friend class api;
		};
	}
}
