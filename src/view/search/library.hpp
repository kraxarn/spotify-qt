#pragma once

#include "lib/spotify/api.hpp"
#include "view/search/tracks.hpp"

namespace View
{
	namespace Search
	{
		class Library: public Tracks
		{
		public:
			Library(lib::spt::api &spotify, lib::cache &cache, QWidget *parent);

			void search(const std::string &query);

		protected:
			void showEvent(QShowEvent *event) override;

		private:
			lib::spt::api &spotify;
		};
	}
}
