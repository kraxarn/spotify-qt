#pragma once

#include "lib/format.hpp"
#include "lib/strings.hpp"
#include "thirdparty/json.hpp"

#include <string>
#include <map>
#include <vector>

namespace lib
{
	namespace spt
	{
		/**
		 * Audio analysis features
		 */
		class audio_features
		{
		public:
			audio_features() = default;

			auto get_values() const -> const std::map<std::string, std::string> &;

			void set(const std::string &key, const std::string &value);

			static auto format_stat(double value,
				const std::initializer_list<std::string> &stages) -> std::string;

			static auto format_stat(double value, const std::string &low,
				const std::string &high) -> std::string;

			static auto format_stat(double value, const std::string &measurement) -> std::string;

			static auto format_volume(double value) -> std::string;

			static auto parse_key(int key) -> std::string;

		private:
			std::map<std::string, std::string> values;

			static constexpr double lowerThreshold = 0.2;
			static constexpr double lowThreshold = 0.4;
			static constexpr double highThreshold = 0.6;
			static constexpr double higherThreshold = 0.8;

			static constexpr double veryQuietDb = -10.0;
			static constexpr double quietDb = -5.0;
		};

		/**
		 * Audio features -> JSON
		 */
		void to_json(nlohmann::json &j, const audio_features &a);

		/**
		 * JSON -> Audio features
		 */
		void from_json(const nlohmann::json &j, audio_features &a);
	}
}
