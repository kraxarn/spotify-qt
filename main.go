package main

import (
	"github.com/therecipe/qt/core"
	"github.com/therecipe/qt/gui"
	"github.com/therecipe/qt/widgets"
	"os"
)

func LayoutToWidget(layout widgets.QLayout_ITF) widgets.QWidget_ITF {
	widget := widgets.NewQWidget(nil, 0)
	widget.SetLayout(layout)
	return widget
}

func MainContent() widgets.QWidget_ITF {
	container := widgets.NewQSplitter(nil)
	return container
}

func main() {
	core.QCoreApplication_SetApplicationName("spotify-qt")

	app := widgets.NewQApplication(len(os.Args), os.Args)
	window := widgets.NewQMainWindow(nil, 0)
	window.SetWindowTitle("spotify-qt")
	window.SetWindowIcon(gui.QIcon_FromTheme("spotify"))
	window.Resize2(1280, 720)
	window.SetCentralWidget(MainContent())

	window.Show()
	app.Exec()
}