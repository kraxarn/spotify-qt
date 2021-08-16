#pragma once

namespace lib
{
	/**
	 * Key of track as pitch
	 * @note I'm no musician, so I have no idea what this means, or if it's accurate
	 * @see https://en.wikipedia.org/wiki/Pitch_class
	 */
	enum class audio_key
	{
		/** C */
		c = 0,

		/** C♯ or D♭ */
		c_sharp = 1,

		/** D */
		d = 2,

		/** D♯ or E♭ */
		d_sharp = 3,

		/** E */
		e = 4,

		/** F */
		f = 5,

		/** F♯ or G♭ */
		f_sharp = 6,

		/** G */
		g = 7,

		/** G♯ or A♭ */
		g_sharp = 8,

		/** A */
		a = 9,

		/** A♯, B♭ */
		a_sharp = 10,

		/** B */
		b = 11,
	};
}
