package main

import (
	"fmt"
	"github.com/therecipe/qt/core"
	"github.com/therecipe/qt/dbus"
)

type MusicPlayer struct {
	itf *dbus.QDBusInterface
}

func NewMusicPlayer() *MusicPlayer {
	// Create media player
	mp := new(MusicPlayer)
	mp.Connect()
	return mp
}

func (mp *MusicPlayer) Connect() {
	go func() {
		fmt.Println("connecting to d-bus...")
		mp.itf = dbus.NewQDBusInterface2(
			"org.mpris.MediaPlayer2.spotifyd",
			"/org/mpris/MediaPlayer2",
			"org.mpris.MediaPlayer2.Player",
			dbus.QDBusConnection_SessionBus(), nil)
		fmt.Println("connected to d-bus")
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