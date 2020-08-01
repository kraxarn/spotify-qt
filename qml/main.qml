import com.kraxarn.spotify 1.0
import com.kraxarn.utils 1.0

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12

import "js/main.js" as MainJs

ApplicationWindow {
	id: root
	title: "spotify-qt-quick"
	visible: true
	width: 540
	height: 960
	
	readonly property bool inPortrait: root.width < root.height

	Utils {
		id: utils
	}

	Spotify {
		id: spotify
		onPlaybackChanged: MainJs.playbackChanged()
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
				icon.name: "speaker"
				icon.source: icSrc("speaker")
				text: "Device"
			}
			ToolButton {
				icon.name: "configure"
				icon.source: icSrc("configure")
				text: "Settings"
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

	Component {
		id: listDelegate
		RowLayout {
			id: listRow
			height: trackList.height / 20
			width: trackList.width - 32
			x: 16
			Label {
				text: model.track
				Layout.preferredWidth: trackList.width / 3
			}
			Label {
				text: model.artist
			}
			Item {
				Layout.fillWidth: true
			}
			ToolButton {
				icon.name: "overflow-menu"
				icon.source: icSrc("overflow-menu")
				flat: true
			}
		}
	}

	Component {
		id: listSeparator
		Rectangle {
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.leftMargin: 16
			anchors.rightMargin: anchors.leftMargin
			height: 1
			color: "#9e9e9e"
			opacity: 0
		}
	}

	ListView {
		id: trackList
		anchors.fill: parent
		anchors.leftMargin: !inPortrait ? drawer.width : 0
		anchors.topMargin: toolBar.height
		section.property: "track"
		section.criteria: ViewSection.FullString
		section.delegate: listSeparator
		model: ListModel {
			id: listModel
			Component.onCompleted: {
				for (var i = 0; i < 200; i++) {
					listModel.append({
						"artist": Math.round(Math.random() * 1024 * 1024).toString(16),
						"track": Math.round(Math.random() * 1024 * 1024).toString(16)
					})
				}
			}
		}
		delegate: listDelegate
	}
}
