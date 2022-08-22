#pragma once

namespace lib
{
	/**
	 * Type of credit for lyrics
	 */
	enum class lyrics_credit
	{
		/**
		 * Unknown type
		 */
		unknown,

		/**
		 * Who provided the lyrics
		 */
		provided,

		lrc,

		/**
		 * Publisher
		 */
		publish,

		/**
		 * Vocalist
		 */
		vocal,

		/**
		 * Guitar
		 */
		guitar,

		program,

		/**
		 * Producer
		 */
		produce,

		/**
		 * Writer
		 */
		write,
	};
}
