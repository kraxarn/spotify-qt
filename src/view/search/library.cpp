#include "view/search/library.hpp"
#include "view/search/search.hpp"

View::Search::Library::Library(lib::spt::api &spotify,
	lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	View::Search::Tracks(spotify, cache, parent)
{
}

void View::Search::Library::search(const std::string &query)
{
	if (query.empty())
	{
		clear();
		return;
	}

	spotify.saved_tracks([this, query](const std::vector<lib::spt::track> &tracks)
	{
		this->clear();
		for (const auto &track : tracks)
		{
			if (lib::strings::contains(lib::strings::to_lower(track.name), query)
				|| lib::strings::contains(lib::strings::to_lower(track.album.name), query)
				|| lib::strings::contains(lib::strings::to_lower(
					lib::spt::entity::combine_names(track.artists)), query))
			{
				this->add(track);
			}
		}
	});
}
