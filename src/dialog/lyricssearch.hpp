#pragma once

#include "dialog/base.hpp"
#include "lib/lyrics//api.hpp"

#include <QGroupBox>
#include <QLineEdit>
#include <QTreeWidget>

namespace Dialog
{
	class LyricsSearch: public Base
	{
	Q_OBJECT

	public:
		LyricsSearch(const HttpClient &httpClient, QWidget *parent);

	private:
		static constexpr int width = 700;
		static constexpr int height = 400;

		static constexpr int widthId = 120;
		static constexpr int widthTitle = 200;
		static constexpr int widthArtist = 180;

		LyricsApi lyrics;

		QGroupBox *searchBox;
		QLineEdit *search;
		QTreeWidget *results;

		void onSearchClicked(bool checked);
		void onResultsItemDoubleClicked(QTreeWidgetItem *item, int);
	};
}
