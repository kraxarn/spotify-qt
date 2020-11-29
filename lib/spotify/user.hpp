#pragma once

#include <string>

namespace lib
{
	namespace spt
	{
		class user
		{
		public:
			user() = default;

			std::string displayName;
			std::string id;
			std::string image;
			std::string product;
		};
	};
}