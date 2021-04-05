#include "lib/spotify/audiofeatures.hpp"

using namespace lib::spt;

void lib::spt::to_json(nlohmann::json &j, const audio_features &a)
{
	j = a.get_values();
}

void lib::spt::from_json(const nlohmann::json &j, audio_features &a)
{
	if (!j.is_object())
	{
		return;
	}

	static constexpr double higherThreshold = 0.8;

	a.set("Acousticness", j.at("acousticness").get<double>() > higherThreshold
		? "Acoustic" : "Not acoustic");

	a.set("Danceability",
		audio_features::format_stat(j.at("danceability"), {
			"Not suitable", "Not very suitable", "Average",
			"Suitable", "Very suitable"
		}));

	a.set("Energy", audio_features::format_stat(j.at("energy"),
		"low", "high"));

	a.set("Instrumentalness",
		audio_features::format_stat(j.at("instrumentalness"),
			"vocals"));

	a.set("Key", audio_features::parse_key(j.at("key")));

	a.set("Liveness", j.at("liveness") > higherThreshold ? "Yes" : "No");

	a.set("Loudness", audio_features::format_volume(j.at("loudness")));

	a.set("Mode", j.at("mode") == 1 ? "Major" : "Minor");

	a.set("Speechiness", audio_features::format_stat(j.at("speechiness"),
		"spoken words"));

	a.set("Tempo", lib::fmt::format("{} BPM", j.at("tempo")));

	a.set("Time signature", j.at("time_signature"));

	a.set("Valence", audio_features::format_stat(j.at("valence"),
		"negative", "positive"));
}

auto audio_features::format_stat(double value,
	const std::initializer_list<std::string> &stages) -> std::string
{
	/*
	 * 0: 00-20
	 * 1: 20-40
	 * 2: 40-60
	 * 3: 60-80
	 * 4: 80-100
	 */
	return std::vector<std::string>(stages)
		.at(value < lowerThreshold ? 0 : value < lowThreshold
			? 1 : value < highThreshold ? 2 : value < higherThreshold ? 3 : 4);
}

auto audio_features::format_stat(double value, const std::string &low,
	const std::string &high) -> std::string
{
	if (value < lowerThreshold || value > higherThreshold)
	{
		return lib::fmt::format("Very {}",
			value < lowerThreshold ? low : high);
	}

	if (value < lowThreshold || value > highThreshold)
	{
		return lib::strings::capitalize(value < lowThreshold ? low : high);
	}

	return "Average";
}

auto audio_features::format_stat(double value, const std::string &measurement) -> std::string
{
	return lib::fmt::format("{} {}", value < lowerThreshold
		? "No" : value < lowThreshold ? "Few" : value < highThreshold
			? "Some" : value < higherThreshold ? "A lot of" : "Only", measurement);
}

auto audio_features::format_volume(double value) -> std::string
{
	return value <= veryQuietDb
		? "Quiet" : value <= quietDb
			? "Average" : "Loud";
}

auto audio_features::parse_key(int key) -> std::string
{
	// https://en.wikipedia.org/wiki/Pitch_class
	switch (key)
	{
		case 0:
			return "C";
		case 1:
			return "C♯, D♭";
		case 2:
			return "D";
		case 3:
			return "D♯, E♭";
		case 4:
			return "E";
		case 5:
			return "F";
		case 6:
			return "F♯, G♭";
		case 7:
			return "G";
		case 8:
			return "G♯, A♭";
		case 9:
			return "A";
		case 10:
			return "A♯, B♭";
		case 11:
			return "B";
		default:
			return "?";
	}
}

auto audio_features::get_values() const -> const std::map<std::string, std::string> &
{
	return values;
}

void audio_features::set(const std::string &key, const std::string &value)
{
	values[key] = value;
}
