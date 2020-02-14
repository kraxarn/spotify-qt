package main

type SpotifyPlaylist struct {
	Collaborative	bool
	Description		string
	ID 				string
	Image 			string
	Name			string
	Public			bool
	Tracks			map[string]interface{}
}

func (pl *SpotifyPlaylist) LoadTracksFromUrl(tracks []SpotifyTrack, url string, offset int, spt *Spotify) error {
	// Load tracks from api
	current, err := spt.Get(url[len("https://api.spotify.com/v1/"):])
	if err != nil {
		return err
	}
	// Load for this url
	items := current["items"].([]interface{})
	for i, track := range items {
		t := track.(map[string]interface{})
		tracks[offset+i] = ParseSpotifyTrack(t)
	}
	// Check if there's a next page
	if current["next"] != nil {
		if err := pl.LoadTracksFromUrl(tracks, current["next"].(string), offset + len(items), spt); err != nil {
			return err
		}
	}
	return nil
}

func (pl *SpotifyPlaylist) LoadTracks(spt *Spotify) ([]SpotifyTrack, error) {
	// Allocate memory for all tracks
	trackList := make([]SpotifyTrack, int(pl.Tracks["total"].(float64)))
	if err := pl.LoadTracksFromUrl(trackList, pl.Tracks["href"].(string), 0, spt); err != nil {
		return nil, err
	}
	return trackList, nil
}