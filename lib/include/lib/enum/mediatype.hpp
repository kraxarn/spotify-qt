#pragma once

namespace lib
{
	/**
	 * Media type
	 */
	enum class media_type
	{
		/** Invalid value */
		unknown,

		/** Audio */
		audio,
	};

	auto to_string(media_type m) -> std::string;

	auto from_string(const std::string &str) -> media_type;
}
