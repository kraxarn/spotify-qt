#pragma once

/*
 * Based of:
 * https://gist.github.com/williamdes/308b95ac9ef1ee89ae0143529c361d37
 */

#include <string>
#include <vector>

namespace lib
{
	/**
	 * Base64 encoding and decoding
	 */
	class base64
	{
	public:
		/**
		 * Encode string as base64
		 * @param str String to encode
		 * @return Encoded string
		 */
		static std::string encode(const std::string &str);

		/**
		 * Decode string from base64
		 * @param str String to decode
		 * @return Decoded string
		 */
		static std::string decode(const std::string &str);

	private:
		/**
		 * Static class
		 */
		base64() = default;

		/**
		 * Get Base64 charset (64 chars)
		 */
		static std::string get_charset();
	};
}