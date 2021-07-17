#pragma once

#include "lib/format.hpp"
#include "lib/strings.hpp"
#include "lib/enum/audiokey.hpp"
#include "lib/enum/audiomode.hpp"
#include "lib/spotify/audiofeature.hpp"
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

			auto items() const -> const std::vector<lib::spt::audio_feature> &;

			void add(::audio_feature feature, float value);
			void add(audio_key key);
			void add(audio_mode mode);

			static auto to_audio_feature(const std::string &feature) -> ::audio_feature;

		private:
			std::vector<lib::spt::audio_feature> values;
		};

		/** JSON -> Audio features */
		void from_json(const nlohmann::json &j, audio_features &a);
	}
}
