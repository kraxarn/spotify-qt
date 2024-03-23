#pragma once

#include "thirdparty/json.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * Page of items
		 * @tparam T Item type
		 */
		template<typename T>
		class page
		{
		public:
			page() = default;

			/**
			 * Next page exists
			 */
			auto has_next() const -> bool
			{
				return !next.empty();
			};

			/**
			 * Items on this page
			 */
			std::vector<T> items;

			/**
			 * Maximum number of items per page
			 */
			int limit = 0;

			/**
			 * Number of offset items
			 */
			int offset = 0;

			/**
			 * Total number of items
			 */
			int total = 0;

			/**
			 * URL for next page
			 */
			std::string next;
		};

		template<typename T>
		void from_json(const nlohmann::json &json, lib::spt::page<T> &page)
		{
			json.at("items").get_to(page.items);
			json.at("limit").get_to(page.limit);
			json.at("total").get_to(page.total);

			if (json.contains("offset"))
			{
				json.at("offset").get_to(page.offset);
			}

			const auto &next = json.at("next");
			if (next.is_string())
			{
				next.get_to(page.next);
			}
		}
	}
}
