#pragma once

#include "api.hpp"

namespace lib
{
	namespace spt
	{
		namespace api
		{
			/**
			 * Wrapper around /playlists/ endpoints
			 */
			class playlists
			{
			public:
				explicit playlists(api &core);

			private:
				api &core;
			};
		}
	}
}
