#include "lib/spotify/audiofeatures.hpp"

auto lib::spt::audio_features::items() const -> const std::vector<lib::spt::audio_feature> &
{
	return values;
}

void lib::spt::audio_features::add(::audio_feature feature, float value)
{
	values.emplace_back(feature, value);
}

void lib::spt::audio_features::add(audio_key key)
{
	values.emplace_back(key);
}

void lib::spt::audio_features::add(audio_mode mode)
{
	values.emplace_back(mode);
}

auto lib::spt::audio_features::to_audio_feature(const std::string &feature) -> ::audio_feature
{
	if (feature == "acousticness")
	{
		return ::audio_feature::acousticness;
	}

	if (feature == "danceability")
	{
		return ::audio_feature::danceability;
	}

	if (feature == "energy")
	{
		return ::audio_feature::energy;
	}

	if (feature == "instrumentalness")
	{
		return ::audio_feature::instrumentalness;
	}

	if (feature == "key")
	{
		return ::audio_feature::key;
	}

	if (feature == "liveness")
	{
		return ::audio_feature::liveness;
	}

	if (feature == "loudness")
	{
		return ::audio_feature::loudness;
	}

	if (feature == "mode")
	{
		return ::audio_feature::mode;
	}

	if (feature == "speechiness")
	{
		return ::audio_feature::speechiness;
	}

	if (feature == "tempo")
	{
		return ::audio_feature::tempo;
	}

	if (feature == "time_signature")
	{
		return ::audio_feature::time_signature;
	}

	if (feature == "valence")
	{
		return ::audio_feature::valence;
	}

	return ::audio_feature::unknown;
}

void lib::spt::to_json(nlohmann::json &j, const audio_features &a)
{
	j = nlohmann::json::array();
	for (const auto &item : a.items())
	{
		j.push_back(item);
	}
}

void lib::spt::from_json(const nlohmann::json &j, audio_features &a)
{
	if (!j.is_object())
	{
		return;
	}

	for (const auto &item : j.items())
	{
		const auto feature = lib::spt::audio_features::to_audio_feature(item.key());
		if (feature == ::audio_feature::unknown)
		{
			continue;
		}

		if (feature == ::audio_feature::key)
		{
			a.add(item.value().get<audio_key>());
			continue;
		}

		if (feature == ::audio_feature::mode)
		{
			a.add(item.value().get<audio_mode>());
			continue;
		}

		a.add(feature, item.value().get<float>());
	}
}
