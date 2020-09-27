#pragma once

// These can be combined (where it makes sense), and therefore uses bit order

enum SpotifyContext
{
	ContextNone = 0,
	ContextAll = 1,
	ContextAlbum = 2,
	ContextArtist = 4,
	ContextPlaylist = 8
};