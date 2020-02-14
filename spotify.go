package main

import (
	"encoding/base64"
	"encoding/json"
	"fmt"
	"io"
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

func (spt *Spotify) Request(url, method string, body io.Reader) (*http.Request, error) {
	// TODO: Check here if access token needs refreshing
	// Prepare request
	req, err := http.NewRequest(method, fmt.Sprintf("https://api.spotify.com/v1/%v", url), body)
	if err != nil {
		return nil, err
	}
	// Set header
	req.Header.Set("Authorization", fmt.Sprintf("Bearer %v", spt.AccessToken()))
	return req, nil
}

func (spt *Spotify) Get(url string) (map[string]interface{}, error) {
	// Prepare get
	req, err := spt.Request(url, http.MethodGet, nil)
	if err != nil {
		return nil, err
	}
	// Send request
	resp, err := spt.httpClient.Do(req)
	if err != nil {
		return nil, err
	}
	defer resp.Body.Close()
	respData, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return nil, err
	}
	// Parse response
	var jsonData map[string]interface{}
	if err = json.Unmarshal(respData, &jsonData); err != nil {
		return nil, err
	}
	return jsonData, nil
}

func (spt *Spotify) Put(url string, body map[string]interface{}) error {
	// Convert body to json
	bodyJson, err := json.Marshal(body)
	if err != nil {
		return err
	}
	// Prepare
	req, err := spt.Request(url, http.MethodPut, strings.NewReader(string(bodyJson)))
	if err != nil {
		return err
	}
	// Send
	resp, err := spt.httpClient.Do(req)
	if err != nil {
		return err
	}
	if resp.StatusCode < 200 || resp.StatusCode > 299 {
		return fmt.Errorf(resp.Status)
	}
	return nil
}

func (spt *Spotify) AccessToken() string {
	return NewSettings().Get("AccessToken", "").(string)
}

func (spt *Spotify) RefreshToken() string {
	return NewSettings().Get("RefreshToken", "").(string)
}

func (spt *Spotify) ClientID() string {
	return os.Getenv("SPOTIFY_QT_ID")
}

func (spt *Spotify) ClientSecret() string {
	return os.Getenv("SPOTIFY_QT_SECRET")
}

func (spt *Spotify) Auth(err chan error) {
	// Check if we already have an access and refresh token
	settings := NewSettings()
	if len(spt.AccessToken()) > 0 && len(spt.RefreshToken()) > 0 {
		fmt.Println("access/refresh token already set, refreshing access token")
		err <- spt.Refresh()
		return
	}
	// Check environment variables
	clientID     := spt.ClientID()
	clientSecret := spt.ClientSecret()
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
			settings.Set("AccessToken",  jsonData["access_token"])
			settings.Set("RefreshToken", jsonData["refresh_token"])
			settings.Sync()
			spt.lastAuth = time.Now()
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

func (spt *Spotify) Refresh() error {
	// Make sure we have a refresh token
	if len(spt.RefreshToken()) <= 0 {
		return fmt.Errorf("refresh token is not set")
	}
	// Create form
	form := url.Values{}
	form.Add("grant_type", "refresh_token")
	form.Add("refresh_token", spt.RefreshToken())
	// Create request
	req, err := http.NewRequest(http.MethodPost,
		"https://accounts.spotify.com/api/token", strings.NewReader(form.Encode()))
	if err != nil {
		return err
	}
	req.Header.Add("Content-Type", "application/x-www-form-urlencoded")
	req.Header.Add("Authorization", fmt.Sprintf("Basic %v",
		base64.StdEncoding.EncodeToString([]byte(fmt.Sprintf("%v:%v", spt.ClientID(), spt.ClientSecret())))))
	// Send request
	response, err := spt.httpClient.Do(req)
	if err != nil {
		return err
	}
	// Get response data
	responseData, err := ioutil.ReadAll(response.Body)
	if err != nil {
		return err
	}
	// Try to parse response
	var jsonData map[string]interface{}
	if err = json.Unmarshal(responseData, &jsonData); err != nil {
		return err
	}
	// Save access token if found
	if accessToken, ok := jsonData["access_token"]; !ok {
		return fmt.Errorf("failed to refresh access token: %v", jsonData["error_description"])
	} else {
		spt.lastAuth = time.Now()
		NewSettings().Set("AccessToken", accessToken)
	}
	return nil
}

func (spt *Spotify) Playlists() []SpotifyPlaylist {
	// Request playlists
	resp, err := spt.Get("me/playlists?limit=50")
	if err != nil {
		return []SpotifyPlaylist{}
	}
	// Parse response as playlists
	items := resp["items"].([]interface{})
	// Create list of playlists
	playlists := make([]SpotifyPlaylist, int(resp["total"].(float64)))
	// Loop through all items and add them to playlist
	for i, item := range items {
		data := item.(map[string]interface{})
		playlists[i] = SpotifyPlaylist{
			Collaborative: data["collaborative"].(bool),
			Description:   data["description"].(string),
			ID:            data["id"].(string),
			Image:         data["images"].([]interface{})[0].(map[string]interface{})["url"].(string),
			Name:          data["name"].(string),
			Public:        data["public"].(bool),
			Tracks:        data["tracks"].(map[string]interface{}),
		}
	}
	return playlists
}

func (spt *Spotify) Devices() []SpotifyDevice {
	resp, err := spt.Get("me/player/devices")
	if err != nil {
		return []SpotifyDevice{}
	}
	allDevices := resp["devices"].([]interface{})
	devices := make([]SpotifyDevice, len(allDevices))
	for i, device := range allDevices {
		data := device.(map[string]interface{})
		devices[i] = SpotifyDevice{
			ID:               data["id"].(string),
			Name:             data["name"].(string),
			Type:             data["type"].(string),
			IsActive:         data["is_active"].(bool),
			IsPrivateSession: data["is_private_session"].(bool),
			IsRestricted:     data["is_restricted"].(bool),
			VolumePercent:    uint8(data["volume_percent"].(float64)),
		}
	}
	return devices
}

func (spt *Spotify) SetDevice(device SpotifyDevice) error {
	return spt.Put("me/player", map[string]interface{}{
		"device_ids": []string{
			device.ID,
		},
	})
}

func (spt *Spotify) PlayTracks(trackIDs []string) error {
	return spt.Put("me/player/play", map[string]interface{}{
		"uris": trackIDs,
	})
}