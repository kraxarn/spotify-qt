#pragma once

#include "lib/settings.hpp"
#include "lib/format.hpp"
#include "lib/spotify/callback.hpp"

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
		 */
		virtual void get(const std::string &url, const headers &headers,
			lib::callback<std::string> &callback) const = 0;

		/**
		 * PUT request
		 * @param body JSON body, or empty if none
		 */
		virtual void put(const std::string &url, const std::string &body,
			const headers &headers, lib::callback<std::string> &callback) const = 0;

		/**
		 * POST request without request body
		 */
		void post(const std::string &url, const headers &headers,
			lib::callback<std::string> &callback) const;

		/**
		 * POST request with request body
		 */
		virtual void post(const std::string &url, const std::string &body,
			const headers &headers, lib::callback<std::string> &callback) const = 0;

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
