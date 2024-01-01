#pragma once

namespace lib
{
	enum class artist_profile: unsigned char
	{
		/**
		 * Unknown value
		 */
		unknown,

		/**
		 * Not social profile
		 */
		string,

		/**
		 * Twitter
		 */
		twitter,

		/**
		 * Instagram
		 */
		instagram,

		/**
		 * Facebook
		 */
		facebook,

		/**
		 * YouTube
		 */
		youtube,

		/**
		 * SoundCloud
		 */
		soundcloud,
	};
}
