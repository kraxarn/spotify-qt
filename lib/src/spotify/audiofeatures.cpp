#include "lib/spotify/audiofeatures.hpp"

auto lib::spt::audio_features::items() const -> const std::vector<lib::spt::audio_feature> &
{
	return values;
}

void lib::spt::audio_features::add(lib::audio_feature feature, float value)
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

auto lib::spt::audio_features::to_audio_feature(const std::string &feature) -> lib::audio_feature
{
	if (feature == "acousticness")
	{
		return lib::audio_feature::acousticness;
	}

	if (feature == "danceability")
	{
		return lib::audio_feature::danceability;
	}

	if (feature == "energy")
	{
		return lib::audio_feature::energy;
	}

	if (feature == "instrumentalness")
	{
		return lib::audio_feature::instrumentalness;
	}

	if (feature == "key")
	{
		return lib::audio_feature::key;
	}

	if (feature == "liveness")
	{
		return lib::audio_feature::liveness;
	}

	if (feature == "loudness")
	{
		return lib::audio_feature::loudness;
	}

	if (feature == "mode")
	{
		return lib::audio_feature::mode;
	}

	if (feature == "speechiness")
	{
		return lib::audio_feature::speechiness;
	}

	if (feature == "tempo")
	{
		return lib::audio_feature::tempo;
	}

	if (feature == "time_signature")
	{
		return lib::audio_feature::time_signature;
	}

	if (feature == "valence")
	{
		return lib::audio_feature::valence;
	}

	return lib::audio_feature::unknown;
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
		if (feature == lib::audio_feature::unknown)
		{
			continue;
		}

		if (feature == lib::audio_feature::key)
		{
			a.add(item.value().get<audio_key>());
			continue;
		}

		if (feature == lib::audio_feature::mode)
		{
			a.add(item.value().get<audio_mode>());
			continue;
		}

		a.add(feature, item.value().get<float>());
	}
}
