import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import "js/playlistView.js" as PlaylistViewJs

ListView {
	id: playlistList

	Component {
		id: playlistItem
		Button {
			height: playlistList.height / 10
			width: playlistList.width
			text: model.name
			flat: true
			onClicked: PlaylistViewJs.clickedPlaylist(model)
		}
	}

	model: ListModel {
		id: listModel
		Component.onCompleted: PlaylistViewJs.loadPlaylists()
	}

	delegate: playlistItem
}