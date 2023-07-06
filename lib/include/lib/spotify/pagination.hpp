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

			/**
			 * Set callback on successful page fetch
			 */
			void success(std::function<void(const std::vector<T> &)> callback)
			{
				ok_callback = std::move(callback);
			}

			/**
			 * Set callback on failed page fetch
			 */
			void error(std::function<void(const std::string &)> callback)
			{
				fail_callback = std::move(callback);
			}

			void next()
			{
				if (!has_next())
				{
					throw std::runtime_error("No next page exists");
				}

				request.get<nlohmann::json>(next_url, [this](const lib::result<nlohmann::json> &response)
				{
					if (!response.success())
					{
						fail(response.message());
						return;
					}

					const auto &json = response.value();
					if (!items_key.empty() && !json.contains(items_key))
					{
						const auto message = lib::fmt::format("No such key: {}", items_key);
						fail(message);
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
					ok(items);
				});
			}

		private:
			std::string next_url;
			std::string items_key;
			request &request;

			std::function<void(const std::vector<T> &)> ok_callback;
			std::function<void(const std::string &)> fail_callback;

			void ok(std::vector<T> items)
			{
				if (!ok_callback)
				{
					lib::log::warn("No success callback set, ignoring");
					return;
				}

				lib::log::debug("OK: {} items fetched", items.size());
				ok_callback(std::move(items));
			}

			void fail(const std::string &message)
			{
				if (!fail_callback)
				{
					lib::log::error("Failed to fetch page: {}", message);
					return;
				}

				lib::log::debug("Fail: {}", message);
				fail_callback(message);
			}
		};
	}
}
