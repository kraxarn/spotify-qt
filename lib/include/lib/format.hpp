#pragma once

#include <string>

#include "lib/spotify/track.hpp"

#include <QString>

namespace lib
{
	/**
	 * Formatting utilities
	 */
	class format
	{
	public:
		/**
		 * \brief Format title template
		 * \param track Track to format for
		 * \param format Template format
		 * \return Formatted title
		 */
		static auto title(const spt::track &track, const std::string &format) -> std::string;
	};
}

/**
 * Formatting utilities
 */
class Format
{
public:
	static constexpr unsigned int kilo = 1'000;
	static constexpr unsigned int mega = 1'000'000;
	static constexpr unsigned int giga = 1'000'000'000;

	/**
	 * Format time as M:SS
	 * @param milliseconds Milliseconds
	 */
	static auto time(int milliseconds) -> QString;

	/**
	 * Format time as "... h ... m"
	 */
	static auto timePretty(int milliseconds) -> QString;

	/**
	 * Format size as B, kB, MB or GB (bytes)
	 * @param bytes Bytes
	 */
	static auto size(unsigned long bytes) -> QString;

	/**
	 * Format as k or M
	 * @param count Amount
	 */
	static auto count(unsigned int count) -> QString;

private:
	Format() = default;
};
