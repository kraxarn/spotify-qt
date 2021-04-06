#pragma once

#include "lib/settings.hpp"
#include "lib/format.hpp"
#include "lib/spotify/callback.hpp"

#include <string>

namespace lib
{
	/**
	 * Abstract HTTP client
	 */
	class http_client
	{
	public:
		virtual void get(const std::string &url,
			lib::callback<std::string> &callback) = 0;

		virtual void put(const std::string &url, const std::string &body,
			lib::callback<std::string> &callback) = 0;

		virtual void post(const std::string &url,
			lib::callback<std::string> &callback) = 0;

		virtual void del(const std::string &url, const std::string &body,
			lib::callback<std::string> &callback) = 0;

	protected:
		explicit http_client(lib::settings &settings);

		/**
		 * Get Authorization header value
		 */
		auto authorization_header() const -> std::string;

		/**
		 * Custom POST request for authorization
		 */
		virtual auto post(const std::string &data,
			const std::string &authorization) -> std::string = 0;

	private:
		lib::settings &settings;
	};
}
