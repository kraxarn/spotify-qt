#pragma once

#include "lib/spotify/request.hpp"
#include <utility>

namespace lib
{
	namespace spt
	{
		/**
		 * Lazy pagination
		 * @tparam T Type of entity contained in each page
		 */
		template<typename T>
		class pagination
		{
		public:
			pagination(const std::string &url, request &request)
				: pagination(url, {}, request)
			{
			}

			pagination(const std::string &url, std::string key, request &request)
				: next_url(lib::spt::to_full_url(url)),
				items_key(std::move(key)),
				request(request)
			{
			}

			auto has_next() const -> bool
			{
				return !next_url.empty();
			}

			auto success() const -> bool
			{
				return error_message.empty();
			}

			void next(lib::callback<lib::result<std::vector<T>>> &callback)
			{
				if (!has_next())
				{
					throw std::runtime_error("No next page exists");
				}

				request.get(next_url, [this, callback](const lib::result<nlohmann::json> &response)
				{
					if (!response.success())
					{
						callback(lib::result<std::vector<T>>::fail(response.message()));
						return;
					}

					const auto &json = response.value();
					if (!items_key.empty() && !json.contains(items_key))
					{
						const auto message = lib::fmt::format("No such key: {}", items_key);
						callback(lib::result<std::vector<T>>::fail(message));
						return;
					}

					const auto &content = items_key.empty() ? json : json.at(items_key);
					if (content.contains("next"))
					{
						const auto &next = content.at("next");
						next_url = next.is_string()
							? next.template get<std::string>()
							: std::string();
					}
					else
					{
						next_url = std::string();
					}

					const auto &items = content.at("items");
					callback(lib::result<std::vector<T>>::ok(items));
				});
			}

		private:
			std::string next_url;
			std::string items_key;
			std::string error_message;
			request &request;
		};
	}
}
