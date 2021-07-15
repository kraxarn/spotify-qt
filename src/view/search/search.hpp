#pragma once

#include "lib/spotify/api.hpp"
#include "lib/httpclient.hpp"

#include "view/search/tracks.hpp"
#include "view/search/artists.hpp"
#include "view/search/albums.hpp"
#include "view/search/playlists.hpp"

#include <QDockWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

namespace View
{
	namespace Search
	{
		class Search: public QWidget
		{
		Q_OBJECT

		public:
			Search(lib::spt::api &spotify, lib::cache &cache,
				const lib::http_client &httpClient, QWidget *parent);

		private:
			QTabWidget *tabs = nullptr;
			QLineEdit *searchBox = nullptr;
			lib::spt::api &spotify;
			lib::cache &cache;
			const lib::http_client &httpClient;

			static constexpr int tracksIndex = 0;
			static constexpr int artistsIndex = 1;
			static constexpr int albumsIndex = 2;
			static constexpr int playlistsIndex = 3;
			static constexpr int libraryIndex = 4;

			View::Search::Tracks *tracks = nullptr;
			View::Search::Artists *artists = nullptr;
			View::Search::Albums *albums = nullptr;
			View::Search::Playlists *playlists = nullptr;

			void search();

		protected:
			void showEvent(QShowEvent *event) override;
			void hideEvent(QHideEvent *event) override;

		private:
			void resultsLoaded(const lib::spt::search_results &results);
		};
	}
}
