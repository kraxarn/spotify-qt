import com.kraxarn.spotify 1.0
import com.kraxarn.utils 1.0
import com.kraxarn.settings 1.0

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12

import "js/main.js" as JS

ApplicationWindow {
	id: root
	title: "spotify-qt-quick"
	visible: true
	width: 540
	height: 960
	Material.theme: Material.System

	readonly property bool inPortrait: root.width < root.height
	readonly property int listItemHeight: 48

	Utils {
		id: utils
	}

	Spotify {
		id: spotify
		onPlaybackChanged: JS.playbackChanged()
	}

	Settings {
		id: settings
	}

	function icSrc(name) {
		return "qrc:/res/ic/dark/%1.svg".arg(name)
	}

	ToolBar {
		id: toolBar
		z: inPortrait ? 0 : 1
		parent: root.overlay
		width: root.width
		RowLayout {
			anchors.fill: parent
			ToolButton {
				icon.name: "application-menu"
				icon.source: icSrc("application-menu")
				visible: inPortrait
				onClicked: function () {
					drawer.visible = !drawer.visible
				}
			}
			Image {
				id: currentAlbum
				Layout.leftMargin: inPortrait ? 0 : 12
				Layout.maximumHeight: currentPlaying.height - 6
				fillMode: Image.PreserveAspectFit
				source: icSrc("media-optical-audio")
			}
			Column {
				id: currentPlaying
				Layout.fillWidth: true
				spacing: -4
				Label {
					id: currentArtist
					text: ""
					font.pixelSize: 14
				}
				Label {
					id: currentTrack
					text: "No media playing"
					font.pixelSize: 20
				}
			}
			ToolButton {
				icon.name: "view-sort"
				onClicked: trackSortMenu.popup()
				Menu {
					id: trackSortMenu
					MenuItem {
						text: "Sort by"
						enabled: false
					}
					MenuItem {
						text: "Track"
						icon.name: "view-media-track"
					}
					MenuItem {
						text: "Artist"
						icon.name: "view-media-artist"
					}
				}
			}
			ToolButton {
				icon.name: "configure"
				icon.source: icSrc("configure")
				onClicked: settingsDrawer.open()
			}
		}
	}

	footer: Footer {}

	Drawer {
		id: drawer
		width: inPortrait ? root.width * 0.66 : 400
		height: root.height - (inPortrait ? 0 : toolBar.height)
		y: inPortrait ? 0 : toolBar.height

		modal: inPortrait
		interactive: inPortrait
		position: inPortrait ? 0 : 1
		visible: !inPortrait

		ColumnLayout {
			anchors.fill: parent
			StackLayout {
				currentIndex: drawerTabs.currentIndex
				Layout.bottomMargin: drawerTabs.height
				SearchView {}
				LibraryView {}
				PlaylistsView {}
			}
			TabBar {
				id: drawerTabs
				Layout.alignment: Qt.AlignBottom
				Layout.fillWidth: true
				Layout.leftMargin: 8
				Layout.rightMargin: 8
				currentIndex: 2
				TabButton {
					icon.name: "search"
					text: "Search"
				}
				TabButton {
					icon.name: "bookmarks-toolbar"
					text: "Library"
				}
				TabButton {
					icon.name: "view-media-playlist"
					text: "Playlists"
				}
			}
			RowLayout {
				Layout.margins: 8
				ToolButton {
					Layout.fillWidth: true
					icon.name: "help-about"
					text: "spotify-qtquick %1".arg(AppVersion)
					enabled: false
				}
				ToolButton {
					icon.name: "im-user-away"
				}
			}
		}
	}

	SettingsDialog {
		id: settingsDrawer
	}

	Component {
		id: listDelegate
		Button {
			id: listRow
			height: listItemHeight
			width: trackList.width
			flat: true
			onClicked: console.log(model.id)
			Label {
				anchors {
					verticalCenter: parent.verticalCenter
					left: parent.left
					leftMargin: 16
				}
				text: model.track
			}
			Label {
				anchors.verticalCenter: parent.verticalCenter
				x: parent.width / 2
				text: model.artist
			}
			ToolButton {
				anchors {
					verticalCenter: parent.verticalCenter
					right: parent.right
					rightMargin: 16
				}
				icon.name: "overflow-menu"
				icon.source: icSrc("overflow-menu")
				flat: true
			}
		}
	}

	ListView {
		id: trackList
		anchors.fill: parent
		anchors.leftMargin: !inPortrait ? drawer.width : 0
		anchors.topMargin: toolBar.height
		model: ListModel {
			id: trackListModel
		}
		delegate: listDelegate
	}
}
