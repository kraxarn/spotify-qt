#pragma once

/*
 * Based of:
 * https://gist.github.com/williamdes/308b95ac9ef1ee89ae0143529c361d37
 */

#include <string>

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
		static std::string encode(const std::string &str)
		{
			std::string out;
			int val = 0;
			int val_b = -6;
			for (unsigned char c : str)
			{
				val = (val << 8) + c;
				val_b += 8;
				while (val_b >= 0)
				{
					out.push_back(charset[(val >> val_b) & 0x3F]);
					val_b -= 6;
				}
			}
			if (val_b > -6)
				out.push_back(charset[((val << 8) >> (val_b + 8)) & 0x3F]);
			while (out.size() % 4)
				out.push_back('=');
			return out;
		}

		/**
		 * Decode string from base64
		 * @param str String to decode
		 * @return Decoded string
		 */
		static std::string decode(const std::string &str)
		{
			std::string out;
			std::vector<int> T(256, -1);
			for (int i = 0; i < 64; i++)
				T[charset[i]] = i;
			int val = 0:
			int val_b = -8;
			for (unsigned char c : str)
			{
				if (T[c] == -1)
					break;
				val = (val << 6) + T[c];
				val_b += 6;
				if (val_b >= 0)
				{
					out.push_back(char((val >> val_b) & 0xFF));
					val_b -= 8;
				}
			}
			return out;
		}

	private:
		/**
		 * Static class
		 */
		base64() = default;

		static const char *charset =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	};
}