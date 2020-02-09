package main

import (
	"fmt"
	"net/http"
	"net/url"
	"os"
	"os/exec"
	"strings"
	"time"
)

type Spotify struct {
	lastAuth time.Time
}

func NewSpotify() *Spotify {
	spt := new(Spotify)
	spt.lastAuth = time.Unix(0, 0)
	return spt
}

func (spt *Spotify) Auth(err chan error) {
	// Check environment variables
	if os.Getenv("SPOTIFY_QT_ID") == "" {
		err <- fmt.Errorf("SPOTIFY_QT_ID is not set")
		return
	}
	if os.Getenv("SPOTIFY_QT_SECRET") == "" {
		err <- fmt.Errorf("SPOTIFY_QT_SECRET is not set")
		return
	}
	// Scopes for the request for clarity
	// For now, these are identical to spotify-tui
	scopes := []string{
		"playlist-read-collaborative",
		"playlist-read-private",
		"playlist-modify-private",
		"playlist-modify-public",
		"user-follow-read",
		"user-follow-modify",
		"user-library-modify",
		"user-library-read",
		"user-modify-playback-state",
		"user-read-currently-playing",
		"user-read-playback-state",
		"user-read-private",
		"user-read-recently-played",
	}
	// Prepare URL and open browser
	authUrl := fmt.Sprintf(
		"https://accounts.spotify.com/authorize?client_id=%v&response_type=code&redirect_uri=%v&scope=%v",
		os.Getenv("SPOTIFY_QT_ID"), url.QueryEscape("http://localhost:8888"), strings.Join(scopes, "%20"))
	// Start a temporary web server to respond to request
	// TODO: This keeps running forever, stop once user has authenticated
	go func() {
		http.HandleFunc("/", func(writer http.ResponseWriter, request *http.Request) {
			_, _ = fmt.Fprintf(writer, "success, you can now go back to spotify-qt")
		})
		if e := http.ListenAndServe(":8888", nil); e != nil {
			err <- e
		}
	}()
	// Open url with xdg-open
	if e := exec.Command("xdg-open", authUrl).Run(); e != nil {
		err <- e
	}
}