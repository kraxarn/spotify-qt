#pragma once

#include <QString>

struct AudioFeatures
{
	int mode, timeSignature;
	QString key;
	double acousticness, danceability, energy, instrumentalness, liveness,
		loudness, speechiness, valence, tempo;
};


