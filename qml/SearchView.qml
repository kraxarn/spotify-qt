import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import "js/searchView.js" as JS

ColumnLayout {
	id: searchView
	Layout.fillHeight: true
	ListView {
		id: listSearch
		Layout.fillHeight: true
		model: ListModel {
			id: searchListModel
		}
		delegate: Component {
			Button {
				height: listItemHeight
				width: searchView.width
				text: model.name
				flat: true
				onClicked: JS.clickedItem(model.id)
			}
		}
	}
	RowLayout {
		id: rowSearch
		TextField {
			id: fieldSearch
			Layout.fillWidth: true
			placeholderText: "Search query"
			onAccepted: JS.search(fieldSearch.text)
		}
		ToolButton {
			id: menuSearchSelectedType
			icon.name: "view-media-track"
			onClicked: menuSearchType.open()
			Menu {
				id: menuSearchType
				onClosed: JS.updateItems()
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