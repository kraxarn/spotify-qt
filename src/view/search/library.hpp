#pragma once

#include "lib/spotify/api.hpp"
#include "view/search/tracks.hpp"

namespace View
{
	namespace Search
	{
		class Library: public Tracks
		{
		Q_OBJECT

		public:
			Library(lib::spt::api &spotify, lib::cache &cache, QWidget *parent);

			/** Searches in cache */
			void searchCache(const std::string &query);

			/** Makes a new request and searches saved tracks */
			void search(const std::string &query);

		private:
			lib::spt::api &spotify;
			lib::cache &cache;
			std::string lastQuery;

			void addResults(const std::string &query,
				const std::vector<lib::spt::track> &tracks);
		};
	}
}
