#pragma once

#include "lib/enum/audiofeature.hpp"
#include "lib/enum/audiokey.hpp"
#include "lib/enum/audiomode.hpp"
#include "lib/log.hpp"

#include <string>

namespace lib
{
	namespace spt
	{
		class audio_feature
		{
		public:
			/**
			 * Audio feature with value
			 * @param feature Category
			 * @param value Value
			 */
			audio_feature(lib::audio_feature feature, float value);

			/** Audio feature with key */
			explicit audio_feature(audio_key key);

			/** Audio feature with mode */
			explicit audio_feature(audio_mode mode);

			/**
			 * Get name of audio feature
			 */
			auto get_feature_string() const -> std::string;

			/**
			 * Get audio feature
			 */
			auto get_feature() const -> lib::audio_feature;

			/**
			 * Get name of value
			 */
			auto get_value_string() const -> std::string;

			/**
			 * Get value
			 */
			auto get_value() const -> float;

			/**
			 * Get minimum possible value
			 */
			auto get_min() const -> float;

			/**
			 * Get maximum possible value
			 */
			auto get_max() const -> float;

			/**
			 * Get value description
			 */
			auto get_description() const -> std::string;

		private:
			lib::audio_feature feature = lib::audio_feature::unknown;
			float minimum = 0.F;
			float maximum = 1.F;
			float value = 0.F;
			std::string name;

			auto acousticness() const -> std::string;
			auto danceability() const -> std::string;
			auto energy() const -> std::string;
			auto instrumentalness() const -> std::string;
			auto liveness() const -> std::string;
			auto loudness(float &min, float &max) const -> std::string;
			auto speechiness() const -> std::string;
			auto tempo(float &max) const -> std::string;
			auto valence() const -> std::string;
			auto time_signature(float &max) const -> std::string;

			static auto to_string(audio_key key) -> std::string;
			static auto to_string(audio_mode mode) -> std::string;
		};
	}
}
