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

func (spt *Spotify) Auth() error {
	// Check environment variables
	if os.Getenv("SPOTIFY_QT_ID") == "" {
		fmt.Println("warning: SPOTIFY_QT_ID is not set")
	}
	if os.Getenv("SPOTIFY_QT_SECRET") == "" {
		fmt.Println("warning: SPOTIFY_QT_SECRET is not set")
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
			fmt.Fprintf(writer, "success, you can now go back to spotify-qt")
		})
		http.ListenAndServe(":8888", nil)
	}()
	// Open url with xdg-open
	return exec.Command("xdg-open", authUrl).Run()
}