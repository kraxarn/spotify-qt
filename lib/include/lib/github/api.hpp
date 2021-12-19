#pragma once

#include "lib/httpclient.hpp"
#include "lib/github/contributor.hpp"
#include "lib/github/release.hpp"

namespace lib
{
	namespace gh
	{
		/**
		 * GitHub API
		 */
		class api
		{
		public:
			api(const lib::http_client &http_client);

			/**
			 * Repository contributors
			 * @param owner Repository owner
			 * @param repo Repository name
			 * @param callback Response callback, empty if failed
			 */
			void contributors(const std::string &owner, const std::string &repo,
				lib::callback<std::vector<lib::gh::contributor>> &callback) const;

			/**
			 * Release information
			 * @param owner Repository owner
			 * @param repo Repository name
			 * @param tag Tag for release to get
			 * @param callback
			 */
			void release(const std::string &owner, const std::string &repo,
				const std::string &tag, lib::callback<lib::gh::release> &callback) const;

		private:
			const lib::http_client &http_client;

			/**
			 * Try to parse response from GitHub
			 * @param response Response from GitHub
			 * @param error_message String to store any error message
			 * @return Parsed JSON, or empty on error
			 */
			static auto parse(const std::string &response,
				std::string &error_message) -> nlohmann::json;
		};
	}
}
