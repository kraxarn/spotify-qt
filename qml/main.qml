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

	Material.theme: Material.Dark
	// Spotify-like colors
	Material.primary: "#282828"
	Material.background: "#121212"
	Material.accent: "#1db954"

	readonly property bool inPortrait: root.width < root.height
	readonly property int listItemHeight: 48
	property string currentContext
	property string currentTrackId

	function loadTracks(tracks, context) {
		drawer.close()
		JS.loadTracks(tracks)
		currentContext = context
	}

	function loadArtist(id) {
		JS.loadArtist(id)
	}

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
				icon.name: "overflow-menu"
				icon.source: icSrc("overflow-menu")
				onClicked: mainMenu.open()
				Menu {
					id: mainMenu
					Menu {
						title: "Sort by"
						MenuItem {
							text: "Track"
							icon.name: "view-media-track"
						}
						MenuItem {
							text: "Artist"
							icon.name: "view-media-artist"
						}
					}
					MenuItem {
						text: "Settings"
						onClicked: settingsDrawer.open()
					}
				}
			}
		}
	}

	footer: Footer {}

	Dialog {
		id: dialogLogOut
		title: "Are you sure?"
		modal: true
		anchors.centerIn: parent
		footer: DialogButtonBox {
			Button {
				text: "Clear everything"
				flat: true
				DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
				onClicked: JS.logOut("clearAll")
			}
			Button {
				text: "Only log out"
				flat: true
				DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
				onClicked: JS.logOut("logOut")
			}
			Button {
				text: "Cancel"
				flat: true
				DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
			}
		}
		Label {
			text: "Do you also want to clear your application credentials or only log out?"
		}
	}

	Dialog {
		id: dialogLoggedOut
		title: "Logged out"
		modal: true
		anchors.centerIn: parent
		standardButtons: Dialog.Ok
		onAccepted: Qt.quit()
		onRejected: Qt.quit()
		Label {
			text: "You are now logged out, the application will now close"
		}
	}

	Drawer {
		id: drawer
		Material.background: Material.primary
		width: inPortrait ? root.width * 0.8 : 400
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
				SearchView {}
				LibraryView {}
				PlaylistsView {}
			}
			TabBar {
				id: drawerTabs
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
					text: "spotify-qt-quick %1".arg(AppVersion)
					enabled: false
				}
				ToolButton {
					icon.name: "im-user-away"
					onClicked: {
						drawer.close()
						dialogLogOut.open()
					}
				}
			}
		}
	}

	SettingsDialog {
		id: settingsDrawer
	}

	ArtistDialog {
		id: artistDrawer
	}

	Component {
		id: listDelegate
		Button {
			id: listRow
			height: listItemHeight
			width: trackList.width
			flat: true
			onClicked: JS.playTrack(model.id)
			readonly property var foregroundColor: currentTrackId === model.id
				? Material.accent : undefined
			Label {
				anchors {
					verticalCenter: parent.verticalCenter
					left: parent.left
					leftMargin: 16
				}
				text: model.track
				Material.foreground: foregroundColor
			}
			Label {
				anchors.verticalCenter: parent.verticalCenter
				x: parent.width / 2
				text: model.artist
				Material.foreground: foregroundColor
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
