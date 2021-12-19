#pragma once

#include "lib/spotify/api.hpp"
#include "lib/httpclient.hpp"

#include "enum/searchtab.hpp"
#include "view/search/tracks.hpp"
#include "view/search/artists.hpp"
#include "view/search/albums.hpp"
#include "view/search/playlists.hpp"
#include "view/search/library.hpp"
#include "view/search/shows.hpp"

#include <QDockWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

namespace Search
{
	class View: public QWidget
	{
	Q_OBJECT

	public:
		View(lib::spt::api &spotify, lib::cache &cache,
			const lib::http_client &httpClient, QWidget *parent);

	private:
		QTabWidget *tabs = nullptr;
		QLineEdit *searchBox = nullptr;
		lib::spt::api &spotify;
		lib::cache &cache;
		const lib::http_client &httpClient;

		Tracks *tracks = nullptr;
		Artists *artists = nullptr;
		Albums *albums = nullptr;
		Playlists *playlists = nullptr;
		Library *library = nullptr;
		Shows *shows = nullptr;

		void search();

	protected:
		void showEvent(QShowEvent *event) override;
		void hideEvent(QHideEvent *event) override;

	private:
		QString searchText;

		void resultsLoaded(const lib::spt::search_results &results);

		void onIndexChanged(int index);
	};
}
