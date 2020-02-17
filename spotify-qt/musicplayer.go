package main

import (
	"fmt"
	godbus "github.com/godbus/dbus"
	"github.com/therecipe/qt/core"
	"github.com/therecipe/qt/dbus"
)

type MusicPlayer struct {
	itf *dbus.QDBusInterface
	bus godbus.BusObject
	isDBus bool

	//metadataMsg *dbus.QDBusMessage
}

type PlayerStatus struct {
	TrackID, Title, Artist, PlaybackStatus, ArtUrl string
	Position, Length int64
}

// Deprecated: Use Web API directly
func NewMusicPlayer() *MusicPlayer {
	// Create media player
	mp := new(MusicPlayer)
	mp.Connect()
	mp.isDBus = false
	return mp
}

func (mp *MusicPlayer) Connect() {
	go func() {
		fmt.Println("connecting to d-bus...")
		// QDBus
		mp.itf = dbus.NewQDBusInterface2(
			"org.mpris.MediaPlayer2.spotifyd",
			"/org/mpris/MediaPlayer2",
			"org.mpris.MediaPlayer2.Player",
			dbus.QDBusConnection_SessionBus(), nil)
		// Go DBus
		conn, _ := godbus.ConnectSessionBus()
		mp.bus = conn.Object("org.mpris.MediaPlayer2.spotifyd", "/org/mpris/MediaPlayer2")
		fmt.Println("connected to d-bus")
		mp.isDBus = true
	}()
}

func (mp *MusicPlayer) Play(trackID string) error {
	result := mp.itf.CallWithArgumentList(0, "OpenUri", []*core.QVariant{
		core.NewQVariant1(fmt.Sprintf("spotify:track:%v", trackID)),
	})
	if len(result.ErrorMessage()) > 0 {
		return fmt.Errorf(result.ErrorMessage())
	}
	return nil
}

func (mp *MusicPlayer) Status() *PlayerStatus {
	// Get status ready
	status := new(PlayerStatus)
	// Get metadata
	metadataVariant, _ := mp.bus.GetProperty("org.mpris.MediaPlayer2.Player.Metadata")
	metadata := metadataVariant.Value().(map[string]godbus.Variant)
	if len(metadata) <= 0 {
		return nil
	}
	// Set values
	status.Artist = metadata["xesam:artist"].Value().([]string)[0]
	status.ArtUrl = metadata["mpris:artUrl"].Value().(string)
	status.Length = metadata["mpris:length"].Value().(int64)
	status.Title  = metadata["xesam:title"].Value().(string)
	// Get position
	status.Position = mp.itf.Property("Position").ToLongLong(nil)
	return status
}