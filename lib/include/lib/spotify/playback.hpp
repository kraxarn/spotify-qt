#pragma once

#include "thirdparty/json.hpp"

namespace lib
{
	namespace spt
	{
		class playback
		{
		public:
			playback() = default;

			nlohmann::json metadata() const;
			int volume() const;

			std::string context_type;
			std::string context_uri;
			std::string repeat = "off";

		};
	}
}