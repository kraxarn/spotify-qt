#pragma once

#include "lib/httpclient.hpp"

#include <cpr/cpr.h>

namespace lib
{
	namespace curl
	{
		class http_client: public lib::http_client
		{
		public:
			void get(const std::string &url, const headers &headers,
				callback<std::string> &callback) const override;

			void put(const std::string &url, const std::string &body, const headers &headers,
				callback<std::string> &callback) const override;

			void post(const std::string &url, const std::string &body, const headers &headers,
				callback<std::string> &callback) const override;

			auto post(const std::string &url, const headers &headers,
				const std::string &post_data) const -> std::string override;

			void del(const std::string &url, const std::string &body, const headers &headers,
				callback<std::string> &callback) const override;

		private:
			static auto cpr_header(const lib::headers &headers) -> cpr::Header;
		};
	}
}
