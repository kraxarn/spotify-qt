package main

import (
	"fmt"
	"github.com/therecipe/qt/core"
	"github.com/therecipe/qt/widgets"
	"os"
)

func main() {
	// Set name for settings etc.
	core.QCoreApplication_SetOrganizationName("kraxarn")
	core.QCoreApplication_SetApplicationName("spotify-qt")
	// Create Qt application
	app := widgets.NewQApplication(len(os.Args), os.Args)
	// Spotify auth
	result := make(chan error)
	spt := NewSpotify()
	go spt.Auth(result)
	if err := <- result; err != nil {
		fmt.Println("error: authentication failed:", err)
	}
	// Create main window
	window := NewMainWindow(spt, app)
	// Show window and run application
	window.window.Show()
	app.Exec()
}