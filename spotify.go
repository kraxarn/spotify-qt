package main

import (
	"encoding/base64"
	"encoding/json"
	"fmt"
	"github.com/therecipe/qt/core"
	"io/ioutil"
	"net/http"
	"net/url"
	"os"
	"os/exec"
	"strings"
	"time"
)

type Spotify struct {
	lastAuth time.Time
	httpClient http.Client
}

func NewSpotify() *Spotify {
	spt := new(Spotify)
	spt.lastAuth = time.Unix(0, 0)
	spt.httpClient = http.Client{}
	return spt
}

func (spt *Spotify) Auth(err chan error) {
	// Check environment variables
	clientID     := os.Getenv("SPOTIFY_QT_ID")
	clientSecret := os.Getenv("SPOTIFY_QT_SECRET")
	if clientID == "" {
		err <- fmt.Errorf("SPOTIFY_QT_ID is not set")
	}
	if clientSecret == "" {
		err <- fmt.Errorf("SPOTIFY_QT_SECRET is not set")
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
	const redirectUrl = "http://localhost:8888"
	authUrl := fmt.Sprintf(
		"https://accounts.spotify.com/authorize?client_id=%v&response_type=code&redirect_uri=%v&scope=%v",
		clientID, url.QueryEscape(redirectUrl), strings.Join(scopes, "%20"))
	// Start a temporary web server to respond to request
	// TODO: This keeps running forever, stop once user has authenticated
	go func() {
		http.HandleFunc("/", func(writer http.ResponseWriter, request *http.Request) {
			// Try to get code parameter
			code := request.URL.Query().Get("code")
			if len(code) <= 0 {
				_, _ = fmt.Fprintf(writer, "sorry, something went wrong")
				err <- fmt.Errorf("code was not received from API")
			}
			// Get values for body
			form := url.Values{}
			form.Add("grant_type", "authorization_code")
			form.Add("code", code)
			form.Add("redirect_uri", redirectUrl)
			r, e := http.NewRequest(
				http.MethodPost, "https://accounts.spotify.com/api/token", strings.NewReader(form.Encode()))
			if e != nil {
				err <- e
				return
			}
			// Set the headers
			r.Header.Add("Content-Type", "application/x-www-form-urlencoded")
			r.Header.Add("Authorization", fmt.Sprintf("Basic %v",
				base64.StdEncoding.EncodeToString([]byte(fmt.Sprintf("%v:%v", clientID, clientSecret)))))
			// Do the request and check result
			response, e := spt.httpClient.Do(r)
			if e != nil {
				err <- e
			}
			responseData, e := ioutil.ReadAll(response.Body)
			if e != nil {
				err <- e
			}
			// Parse the result as json
			var jsonData map[string]interface{}
			if e = json.Unmarshal(responseData, &jsonData); e != nil {
				err <- e
			}
			// Check if json returned an error
			if jsonErr, ok := jsonData["error_description"]; ok {
				err <- fmt.Errorf(jsonErr.(string))
			}
			// Otherwise, assume everything went fine and save token
			settings := core.NewQSettings5(nil)
			settings.SetValue("AccessToken",  core.NewQVariant1(jsonData["access_token"]))
			settings.SetValue("RefreshToken", core.NewQVariant1(jsonData["refresh_token"]))
			settings.Sync()
			// Respond to client everything went fine
			_, _ = fmt.Fprintf(writer, "success, you can now go back to spotify-qt")
			err <- nil
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