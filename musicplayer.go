package main

import (
	"fmt"
	"github.com/godbus/dbus"
)

type MusicPlayer struct {
	bus *dbus.Conn
	obj dbus.BusObject
}

func NewMusicPlayer() *MusicPlayer {
	// Create media player
	mp := new(MusicPlayer)
	mp.Connect()
	return mp
}

func (mp *MusicPlayer) Connect() {
	var err error
	// Connect to bus
	if mp.bus != nil {
		mp.bus.Close()
		mp.bus = nil
	}
	mp.bus, err = dbus.ConnectSessionBus()
	if err != nil {
		fmt.Println("error: failed to connect to session bus:", err)
		return
	}
	// Prepare the object to media player
	mp.obj = mp.bus.Object("org.mpris.MediaPlayer2.spotifyd", "/org/mpris/MediaPlayer2")
}

func (mp *MusicPlayer) Play(trackID string) error {
	return mp.obj.Call("org.mpris.MediaPlayer2.Player.OpenUri", dbus.FlagNoReplyExpected,
		fmt.Sprintf("spotify:track:%v", trackID)).Err
}