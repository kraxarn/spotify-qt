#pragma once

namespace lib
{
	/**
	 * Relation between album and artist
	 */
	enum class album_group: unsigned char
	{
		/**
		 * Artist's album
		 */
		album,

		/**
		 * Album with a single
		 */
		single,

		/**
		 * Compilation from other albums
		 */
		compilation,

		/**
		 * Artist appears in the album, while not being primary artist
		 */
		appears_on,

		/**
		 * Album isn't associated with an artist, or not available from the API
		 */
		none,
	};
}
