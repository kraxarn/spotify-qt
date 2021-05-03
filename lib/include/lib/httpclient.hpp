#pragma once

#include "lib/settings.hpp"
#include "lib/format.hpp"
#include "lib/spotify/callback.hpp"

#include <string>

namespace lib
{
	using headers = std::map<std::string, std::string>;

	/**
	 * Abstract HTTP client
	 */
	class http_client
	{
	public:
		virtual void get(const std::string &url,
			const headers &headers,
			lib::callback<std::string> &callback) = 0;

		virtual void put(const std::string &url, const std::string &body,
			const headers &headers,
			lib::callback<std::string> &callback) = 0;

		virtual void post(const std::string &url,
			const headers &headers,
			lib::callback<std::string> &callback) = 0;

		virtual void del(const std::string &url, const std::string &body,
			const headers &headers,
			lib::callback<std::string> &callback) = 0;

	protected:
		explicit http_client(lib::settings &settings);

	private:
		/**
		 * Get Authorization header value
		 */
		auto authorization_header() const -> std::string;

		lib::settings &settings;
	};
}
