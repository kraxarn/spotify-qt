import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import "js/playlistView.js" as JS

ListView {
	id: playlistList

	Component {
		id: playlistItem
		Button {
			height: playlistList.height / 10
			width: playlistList.width
			text: model.text
			flat: true
		}
	}

	model: ListModel {
		id: listModel
		Component.onCompleted: JS.loadPlaylists()
	}

	delegate: playlistItem
}