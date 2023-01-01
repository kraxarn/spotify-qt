#pragma once

#include "lib/settings.hpp"
#include "lib/format.hpp"
#include "lib/spotify/callback.hpp"
#include "lib/result.hpp"

#include <string>

namespace lib
{
	/**
	 * Request auth_headers
	 */
	using headers = std::map<std::string, std::string>;

	/**
	 * Abstract HTTP client
	 */
	class http_client
	{
	public:
		/**
		 * GET request
		 * @deprecated Use with result callback instead
		 */
		virtual void get(const std::string &url, const headers &headers,
			lib::callback<std::string> &callback) const = 0;

		/**
		 * GET request
		 */
		virtual void get(const std::string &url, const lib::headers &headers,
			lib::callback<lib::result<std::string>> &callback) const = 0;

		/**
		 * PUT request
		 * @param body JSON body, or empty if none
		 */
		virtual void put(const std::string &url, const std::string &body,
			const headers &headers, lib::callback<std::string> &callback) const = 0;

		/**
		 * POST request without request body
		 * @deprecated Use with result callback instead
		 */
		void post(const std::string &url, const headers &headers,
			lib::callback<std::string> &callback) const;

		/**
		 * POST request without request body
		 */
		void post(const std::string &url, const headers &headers,
			lib::callback<lib::result<std::string>> &callback) const;

		/**
		 * POST request with request body
		 * @deprecated Use with result callback instead
		 */
		virtual void post(const std::string &url, const std::string &body,
			const headers &headers, lib::callback<std::string> &callback) const = 0;

		/**
		 * POST request with request body
		 */
		virtual void post(const std::string &url, const std::string &body,
			const headers &headers, lib::callback<lib::result<std::string>> &callback) const = 0;

		/**
		 * Synchronous POST request
		 * @deprecated Use asynchronous method
		 */
		virtual auto post(const std::string &url, const headers &headers,
			const std::string &post_data) const -> std::string = 0;

		/**
		 * DELETE request
		 * @param body JSON body, or empty if none
		 */
		virtual void del(const std::string &url, const std::string &body,
			const headers &headers, lib::callback<std::string> &callback) const = 0;
	};
}
