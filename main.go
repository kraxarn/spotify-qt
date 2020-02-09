package main

import (
	"github.com/therecipe/qt/core"
	"github.com/therecipe/qt/widgets"
	"os"
)

func main() {
	// Set name for settings etc.
	core.QCoreApplication_SetApplicationName("spotify-qt")
	// Create Qt application and main window
	app := widgets.NewQApplication(len(os.Args), os.Args)
	window := NewMainWindow()
	// Show window and run application
	window.window.Show()
	app.Exec()
}