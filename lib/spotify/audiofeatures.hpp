#pragma once

#include "lib/json.hpp"
#include "format.hpp"

namespace lib
{
	namespace spt
	{
		class AudioFeatures
		{
		public:
			explicit AudioFeatures(const nlohmann::json &json);
			std::map<std::string, std::string> values;

		private:
			static std::string formatStat(double value, const std::vector<std::string> &stages);
			static std::string formatStat(double value, const std::string &low, const std::string &high);
			static std::string formatStat(double value, const std::string &measurement);
			static std::string formatVolume(double value);
			static std::string parseKey(int key);
		};
	}
}