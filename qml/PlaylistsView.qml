import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import "js/playlistView.js" as JS

ListView {
	id: playlistList

	Component.onCompleted: JS.loadPlaylist(settings.general.last_playlist)

	Component {
		id: playlistItem
		Button {
			height: listItemHeight
			width: playlistList.width
			text: model.name
			flat: true
			onClicked: JS.clickedPlaylist(model)
			highlighted: root.currentContext === `spotify:playlist:${model.id}`
		}
	}

	model: ListModel {
		id: listModel
		Component.onCompleted: JS.loadPlaylists()
	}

	delegate: playlistItem
}