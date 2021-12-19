#pragma once

#include "lib/spotify/artist.hpp"
#include "util/icon.hpp"

#include <QMenu>
#include <QClipboard>

namespace Artist
{
	class ShareMenu: public QMenu
	{
	public:
		ShareMenu(const lib::spt::artist &artist, QWidget *parent);

	private:
		const lib::spt::artist &artist;

		void onCopyArtistLink(bool checked);
		void onOpenInSpotify(bool checked);
	};
}
