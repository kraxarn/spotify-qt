package main

import "time"

type SpotifyTrack struct {
	AddedAt time.Time
	IsLocal bool
	Track map[string]interface{}
}

func (tck *SpotifyTrack) ID() string {
	id, found := tck.Track["id"]
	if !found || id == nil {
		return "0"
	}
	return id.(string)
}

func (tck *SpotifyTrack) Album() string {
	album, found := tck.Track["album"]
	if !found {
		return "(no album)"
	}
	return album.(map[string]interface{})["name"].(string)
}

func (tck *SpotifyTrack) Artist() string {
	artists, found := tck.Track["artists"]
	if !found {
		return "(no artist)"
	}
	return artists.([]interface{})[0].(map[string]interface{})["name"].(string)
}

func (tck *SpotifyTrack) Name() string {
	name, found := tck.Track["name"]
	if !found {
		return "(no name)"
	}
	return name.(string)
}

func (tck *SpotifyTrack) Duration() uint {
	duration, found := tck.Track["duration_ms"]
	if !found {
		return 0
	}
	return uint(duration.(float64))
}

func (tck *SpotifyTrack) Image() string {
	return tck.Track["album"].(map[string]interface{})["images"].([]interface{})[0].(map[string]interface{})["url"].(string)
}