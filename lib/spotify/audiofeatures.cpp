#include "audiofeatures.hpp"

using namespace lib::spt;

AudioFeatures::AudioFeatures(const nlohmann::json &json)
{
	values["Acousticness"] = json.at("acousticness").get<double>() > 0.8
		? "Acoustic"
		: "Not acoustic";
	values["Danceability"] = formatStat(json.at("danceability").get<double>(), {
		"Not suitable", "Not very suitable", "Average",
		"Suitable", "Very suitable"
	});
	values["Energy"] = formatStat(json.at("energy").get<double>(), "low", "high");
	values["Instrumentalness"] = formatStat(json.at("instrumentalness").get<double>(), "vocals");
	values["Key"] = parseKey(json.at("key").get<int>());
	values["Liveness"] = json.at("liveness").get<double>() > 0.8 ? "Yes" : "No";
	values["Loudness"] = formatVolume(json.at("loudness").get<double>());
	values["Mode"] = json.at("mode").get<int>() == 1 ? "Major" : "Minor";
	values["Speechiness"] = formatStat(json.at("speechiness").get<double>(), "spoken words");
	values["Tempo"] = fmt::format("{} BPM", json.at("tempo").get<double>());
	values["Time signature"] = fmt::format("{}", json.at("time_signature").get<int>());
	values["Valence"] = formatStat(json.at("valence").get<double>(), "negative", "positive");
}

std::string AudioFeatures::formatStat(double value, const std::vector<std::string> &stages)
{
	/*
	 * 0: 00-20
	 * 1: 20-40
	 * 2: 40-60
	 * 3: 60-80
	 * 4: 80-100
	 */
	return stages[value < 0.2 ? 0 : value < 0.4 ? 1 : value < 0.6 ? 2 : value < 0.8 ? 3 : 4];
}

std::string AudioFeatures::formatStat(double value, const std::string &low, const std::string &high)
{
	if (value < 0.2 || value > 0.8)
		return fmt::format("Very {}", value < 0.2 ? low : high);
	if (value < 0.4 || value > 0.6)
	{
		auto str = value < 0.4 ? low : high;
		return fmt::format("{}{}", std::toupper(str[0]), str.substr(1));
	}
	return "Average";
}

std::string AudioFeatures::formatStat(double value, const std::string &measurement)
{
	return fmt::format("{} {}",
		value < 0.2 ? "No" : value < 0.4 ? "Few" : value < 0.6 ? "Some" : value < 0.8 ? "A lot of" : "Only",
		measurement);
}

std::string AudioFeatures::formatVolume(double value)
{
	return value <= -10 ? "Quiet" : value <= -5 ? "Average" : "Loud";
}

std::string AudioFeatures::parseKey(int key)
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