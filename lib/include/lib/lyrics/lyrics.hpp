#pragma once

#include "lib/lyrics/line.hpp"
#include "lib/lyrics/credit.hpp"

#include "thirdparty/json.hpp"

namespace lib
{
	namespace lrc
	{
		class lyrics
		{
		public:
			std::vector<line> lines;
			std::vector<credit> credits;

			/**
			 * Lyrics are synced. and have timestamps
			 */
			auto is_synced() const -> bool;
		};

		void from_json(const nlohmann::json &json, lyrics &lyrics);
	}
}
