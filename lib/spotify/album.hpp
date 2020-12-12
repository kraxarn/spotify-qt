#pragma once

#include "../lib/json.hpp"
#include "util/date.hpp"

#include <iomanip>
#include <sstream>

namespace lib
{
	namespace spt
	{
		class Album
		{
		public:
			Album() = default;
			explicit Album(const nlohmann::json &json);

			nlohmann::json toJson() const;

			std::string albumGroup;
			std::string artist;
			std::string id;
			std::string image;
			std::string name;
			date releaseDate;
		};
	}
}