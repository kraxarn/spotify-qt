#pragma once

#include "lib/lyrics/line.hpp"

#include "thirdparty/json.hpp"

namespace lib
{
	namespace lrc
	{
		class lyrics
		{
		public:
			std::vector<line> lines;
		};

		void from_json(const nlohmann::json &json, lyrics &lyrics);
	}
}
