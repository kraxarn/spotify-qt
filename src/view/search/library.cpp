#include "view/search/library.hpp"
#include "view/search/search.hpp"

View::Search::Library::Library(lib::spt::api &spotify,
	lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	cache(cache),
	View::Search::Tracks(spotify, cache, parent)
{
}

void View::Search::Library::searchCache(const std::string &query)
{
	if (query.empty())
	{
		clear();
		return;
	}

	addResults(query, cache.get_tracks("liked_tracks"));
}

void View::Search::Library::search(const std::string &query)
{
	// To avoid an "unnecessary" request, don't search the same thing twice
	if (query == lastQuery)
	{
		return;
	}
	lastQuery = query;

	if (query.empty())
	{
		clear();
		return;
	}

	spotify.saved_tracks([this, query](const std::vector<lib::spt::track> &tracks)
	{
		this->addResults(query, tracks);
	});
}

void View::Search::Library::addResults(const std::string &query,
	const std::vector<lib::spt::track> &tracks)
{
	clear();
	auto q = lib::strings::to_lower(query);

	for (const auto &track : tracks)
	{
		if (lib::strings::contains(lib::strings::to_lower(track.name), q)
			|| lib::strings::contains(lib::strings::to_lower(track.album.name), q)
			|| lib::strings::contains(lib::strings::to_lower(
				lib::spt::entity::combine_names(track.artists)), q))
		{
			add(track);
		}
	}
}
