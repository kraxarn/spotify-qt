#pragma once

#include "lib/httpclient.hpp"
#include "lib/spotify/artist.hpp"
#include "lib/search/results.hpp"

namespace lib
{
	namespace ddg
	{
		class api
		{
		public:
			api(const lib::http_client &http_client);

			void search(const lib::spt::artist &artist, lib::callback<lib::ddg::results> &callback);

		private:
			const lib::http_client &http;
		};
	}
}
