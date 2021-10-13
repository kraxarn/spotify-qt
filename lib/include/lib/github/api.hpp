#pragma once

#include "lib/httpclient.hpp"
#include "lib/github/contributor.hpp"

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
				lib::callback<std::vector<lib::gh::contributor>> &callback);

		private:
			const lib::http_client &http_client;
		};
	}
}
