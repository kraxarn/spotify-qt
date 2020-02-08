package main

import (
	"fmt"
	"github.com/therecipe/qt/core"
	"github.com/therecipe/qt/gui"
	"github.com/therecipe/qt/widgets"
	"os"
)

func LayoutToWidget(layout widgets.QLayout_ITF) *widgets.QWidget {
	widget := widgets.NewQWidget(nil, 0)
	widget.SetLayout(layout)
	return widget
}

func CreateGroupBox(widget ...widgets.QWidget_ITF) *widgets.QGroupBox {
	group := widgets.NewQGroupBox(nil)
	layout := widgets.NewQVBoxLayout()
	for _, w := range widget {
		layout.AddWidget(w, 0, 0)
	}
	group.SetLayout(layout)
	return group
}

func MainToolBar() widgets.QToolBar_ITF {
	toolBar := widgets.NewQToolBar2(nil)
	toolBar.SetMovable(false)
	// Media controls
	toolBar.AddAction2(gui.QIcon_FromTheme("media-skip-backward"),  "Previous")
	toolBar.AddAction2(gui.QIcon_FromTheme("media-playback-start"), "Play")
	toolBar.AddAction2(gui.QIcon_FromTheme("media-playback-stop"),  "Stop")
	toolBar.AddAction2(gui.QIcon_FromTheme("media-skip-forward"),   "Next")
	// Progress
	progress := widgets.NewQSlider(nil)
	progress.SetOrientation(0x1)
	toolBar.AddSeparator()
	toolBar.AddWidget(progress)
	toolBar.AddSeparator()
	toolBar.AddWidget(widgets.NewQLabel2("0:00/0:00", nil, 0))
	toolBar.AddSeparator()
	// Repeat and shuffle toggles
	toolBar.AddAction2(gui.QIcon_FromTheme("media-playlist-repeat"),   "Repeat").SetCheckable(true)
	toolBar.AddAction2(gui.QIcon_FromTheme("media-playlist-shuffle"),  "Shuffle").SetCheckable(true)
	// Volume slider
	volume := widgets.NewQSlider(nil)
	volume.SetOrientation(0x1)
	volume.SetMaximumWidth(100)
	volume.SetMinimum(0)
	volume.SetMaximum(20)
	volume.SetValue(20)
	toolBar.AddWidget(volume)

	return toolBar
}

func MainContent() widgets.QWidget_ITF {
	container := widgets.NewQSplitter(nil)

	// Sidebar with playlists etc.
	sidebar := widgets.NewQVBoxLayout()
	libraryList := widgets.NewQListWidget(nil)
	libraryList.AddItems([]string{
		"Made For You", "Recently Played", "Liked Songs", "Albums", "Artists",
	})
	library := CreateGroupBox(libraryList)
	library.SetTitle("Library")
	sidebar.AddWidget(library, 0, 0)

	playlistsList := widgets.NewQListWidget(nil)
	playlistsList.AddItems([]string{
		"p01", "p02", "p03", "p04", "p05", "p06", "p07", "p08", "p09",
	})
	playlists := CreateGroupBox(playlistsList)
	playlists.SetTitle("Playlists")
	sidebar.AddWidget(playlists, 1, 0)

	sidebarWidget := LayoutToWidget(sidebar)
	sidebarWidget.SetMaximumWidth(250)
	container.AddWidget(sidebarWidget)

	// Table with songs
	songs := widgets.NewQTreeWidget(nil)
	songs.SetColumnCount(4)
	songs.SetEditTriggers(0)
	songs.SetSelectionBehavior(1)
	songs.SetSortingEnabled(true)
	songs.SetRootIsDecorated(false)
	songs.SetAllColumnsShowFocus(true)
	songs.SortByColumn(0, 0)
	headers := []string{
		"Title", "Artist", "Album", "Length",
	}
	songs.SetHeaderLabels(headers)
	for i := range headers {
		songs.SetColumnWidth(i, 200)
	}
	for i := 0; i < 20; i++ {
		item := widgets.NewQTreeWidgetItem2([]string{
			fmt.Sprintf("song%02v-title",  i),
			fmt.Sprintf("song%02v-artist", i),
			fmt.Sprintf("song%02v-album",  i),
			fmt.Sprintf("song%02v-length", i),
		}, 0)
		if i == 0 {
			item.SetIcon(0, gui.QIcon_FromTheme("media-playback-start"))
		} else {
			item.SetIcon(0, gui.NewQIcon())
		}
		songs.InsertTopLevelItem(i, item)

	}
	container.AddWidget(songs)

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
	window.AddToolBar(0x4, MainToolBar())

	window.Show()
	app.Exec()
}