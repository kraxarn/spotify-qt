package main

import (
	"fmt"
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

func CreateGroupBox(widget ...widgets.QWidget_ITF) *widgets.QGroupBox {
	group := widgets.NewQGroupBox(nil)
	layout := widgets.NewQVBoxLayout()
	for _, w := range widget {
		layout.AddWidget(w, 0, 0)
	}
	group.SetLayout(layout)
	return group
}

func CreateRow(table *widgets.QTableWidget, row int, title, artist, album, length string) {
	rowItems := []*widgets.QTableWidgetItem{
		widgets.NewQTableWidgetItem2(title,  0),
		widgets.NewQTableWidgetItem2(artist, 1),
		widgets.NewQTableWidgetItem2(album,  2),
		widgets.NewQTableWidgetItem2(length, 3),
	}
	for index, item := range rowItems {
		table.SetItem(row, index, item)
	}
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

	container.AddWidget(LayoutToWidget(sidebar))

	// Table with songs
	songs := widgets.NewQTableWidget(nil)
	songs.SetColumnCount(4)
	songs.SetHorizontalHeaderLabels([]string{
		"Title", "Artist", "Album", "Length",
	})
	for i := 0; i < 20; i++ {
		CreateRow(songs, 0,
			fmt.Sprintf("song%v-title",  i),
			fmt.Sprintf("song%v-artist", i),
			fmt.Sprintf("song%v-album",  i),
			fmt.Sprintf("song%v-length", i))
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

	window.Show()
	app.Exec()
}