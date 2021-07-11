#include "view/artist/searchmenu.hpp"

View::Artist::SearchMenu::SearchMenu(const lib::spt::artist &artist, QWidget *parent)
	: artist(artist),
	QMenu(parent)
{
	setIcon(Icon::get("edit-find"));
	setTitle("Search");

	auto *wikipedia = addAction("Wikipedia");
	QAction::connect(wikipedia, &QAction::triggered,
		this, &View::Artist::SearchMenu::onWikipedia);

	auto *duckDuckGo = addAction("DuckDuckGo");
	QAction::connect(duckDuckGo, &QAction::triggered,
		this, &View::Artist::SearchMenu::onDuckDuckGo);
}

void View::Artist::SearchMenu::onWikipedia(bool /*checked*/)
{
	const auto url = lib::fmt::format(
		"https://www.wikipedia.org/search-redirect.php?family=wikipedia&go=Go&search={}",
		artist.name);
	UrlUtils::open(url, LinkType::Web, this);
}

void View::Artist::SearchMenu::onDuckDuckGo(bool /*checked*/)
{
	const auto url = lib::fmt::format("https://duckduckgo.com/?t=h_&q={}", artist.name);
	UrlUtils::open(url, LinkType::Web, this);
}
