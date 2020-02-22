#include "audiofeatures.hpp"

using namespace spt;

AudioFeatures::AudioFeatures(const QJsonObject &json)
{
	values["Acousticness"] = json["acousticness"].toDouble() > 0.8 ? "Acoustic" : "Not acoustic";
	values["Danceability"] = formatStat(json["danceability"].toDouble(), {
		"Not suitable", "Not very suitable", "Average",
		"Suitable", "Very suitable"
	});
	values["Energy"] = formatStat(json["energy"].toDouble(), "low", "high");
	values["Instrumentalness"] = formatStat(json["instrumentalness"].toDouble(), "vocals");
	values["Key"] = parseKey(json["key"].toInt());
	values["Liveness"] = json["liveness"].toDouble() > 0.8 ? "Yes" : "No";
	values["Loudness"] = formatVolume(json["loudness"].toDouble());
	values["Mode"] = json["mode"].toInt() == 1 ? "Major" : "Minor";
	values["Speechiness"] = formatStat(json["speechiness"].toDouble(), "spoken words");
	values["Tempo"] = QString("%1 BPM").arg(json["tempo"].toDouble(), 0, 'f', 0);
	values["Time signature"] = QString::number(json["time_signature"].toInt());
	values["Valence"] = formatStat(json["valence"].toDouble(), "negative", "positive");
}

QString AudioFeatures::formatStat(double value, const QStringList &stages)
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

QString AudioFeatures::formatStat(double value, const QString &low, const QString &high)
{
	if (value < 0.2 || value > 0.8)
		return QString("Very %1").arg(value < 0.2 ? low : high);
	if (value < 0.4 || value > 0.6)
		return QString("%1%2")
			.arg((value < 0.4 ? low : high)[0].toUpper())
			.arg(QString(value < 0.4 ? low : high).remove(0, 1));
	return "Average";
}

QString AudioFeatures::formatStat(double value, const QString &measurement)
{
	return QString("%1 %2")
		.arg(value < 0.2 ? "No" : value < 0.4 ? "Few" : value < 0.6 ? "Some" : value < 0.8 ? "A lot of" : "Only")
		.arg(measurement);
}

QString AudioFeatures::formatVolume(double value)
{
	return value <= -10 ? "Quiet" : value <= -5 ? "Average" : "Loud";
}

QString AudioFeatures::parseKey(int key)
{
	// https://en.wikipedia.org/wiki/Pitch_class
	switch (key)
	{
		case 0:  return "C";
		case 1:  return "C♯, D♭";
		case 2:  return "D";
		case 3:  return "D♯, E♭";
		case 4:  return "E";
		case 5:  return "F";
		case 6:  return "F♯, G♭";
		case 7:  return "G";
		case 8:  return "G♯, A♭";
		case 9:  return "A";
		case 10: return "A♯, B♭";
		case 11: return "B";
		default: return "?";
	}
}