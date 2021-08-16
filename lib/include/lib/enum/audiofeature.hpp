#pragma once

namespace lib
{
	/**
	 * Category of audio analysis
	 */
	enum class audio_feature
	{
		/**
		 * Invalid, or missing, value
		 */
		unknown,

		/**
		 * Confidence track is acoustic
		 * @return 0.0 - 1.0
		 */
		acousticness,

		/**
		 * How suitable track is for dancing
		 * @return 0.0 - 1.0
		 */
		danceability,

		/**
		 * Track's intensity and activity
		 * @return 0.0 - 1.0
		 */
		energy,

		/**
		 * Prediction of no vocals
		 * @return 0.0 - 1.0
		 */
		instrumentalness,

		/**
		 * Key track is in
		 * @return audio_key
		 */
		key,

		/**
		 * Detects audience in recording
		 * @return 0.8> if track is live
		 */
		liveness,

		/**
		 * Average volume
		 * @return -60.0 - 0.0 dB
		 */
		loudness,

		/**
		 * Track modality
		 * @return audio_mode
		 */
		mode,

		/**
		 * Presence of spoken words (not vocals)
		 * @return <0.33 if music, 0.33 - 0.66 if music and speech, 0.66> if speech
		 */
		speechiness,

		/**
		 * Tempo
		 * @return BPM
		 */
		tempo,

		/**
		 * Overall time signature
		 * @return meter
		 */
		time_signature,

		/**
		 * How positive/happy the track is
		 * @return 0.0 - 1.0
		 */
		valence,
	};
}
