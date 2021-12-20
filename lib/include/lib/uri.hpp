#pragma once

#include "lib/strings.hpp"

namespace lib
{
	/**
	 * Basic URI manipulation
	 */
	class uri
	{
	public:
		/**
		 * Create new instance from URL
		 */
		explicit uri(const std::string &url);

		/**
		 * Get URL
		 */
		auto get_url() const -> const std::string &;

		/**
		 * Set URL, validates and throws on failure
		 */
		void set_url(const std::string &url);

		/**
		 * Hostname, for example, "example.com"
		 */
		auto hostname() -> std::string;

		/**
		 * Origin, for example, "https://example.com"
		 */
		auto origin() -> std::string;

		/**
		 * Pathname, for example, /page
		 */
		auto pathname() -> std::string;

		/**
		 * Protocol, probably "http:" or "https:"
		 */
		auto protocol() -> std::string;

		/**
		 * Get map of all search parameters
		 */
		auto get_search_params() -> std::map<std::string, std::string>;

		/**
		 * Replace all search parameters with the set values
		 */
		void set_search_params(const std::map<std::string, std::string> &params);

	private:
		std::string url;
	};
}
