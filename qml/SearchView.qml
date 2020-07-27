import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
	Layout.fillHeight: true
	RowLayout {
		anchors {
			bottom: parent.bottom
			left: parent.left
			right: parent.right
			margins: 16
		}
		TextField {
			Layout.fillWidth: true
			placeholderText: "Search query"
		}
		ToolButton {
			id: menuSearchSelectedType
			icon.name: "view-media-track"
			onClicked: menuSearchType.open()
			Menu {
				id: menuSearchType
				onClosed: console.log(menuSearchSelectedType.icon.name)
				MenuItem {
					text: "Tracks"
					icon.name: "view-media-track"
					onClicked: menuSearchSelectedType.icon.name = "view-media-track"
				}
				MenuItem {
					text: "Artists"
					icon.name: "view-media-artist"
					onClicked: menuSearchSelectedType.icon.name = "view-media-artist"
				}
				MenuItem {
					text: "Albums"
					icon.name: "view-media-album-cover"
					onClicked: menuSearchSelectedType.icon.name = "view-media-album-cover"
				}
				MenuItem {
					text: "Playlists"
					icon.name: "view-media-playlist"
					onClicked: menuSearchSelectedType.icon.name = "view-media-playlist"
				}
			}
		}
	}
}