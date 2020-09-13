import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12

import "js/artistDialog.js" as JS

Drawer {
	id: rootArtist
	y: toolBar.height
	width: parent.width
	height: parent.height - toolBar.height
	edge: Qt.RightEdge

	function load(data) {
		JS.load(data)
	}

	Item {
		anchors.fill: parent
		Image {
			id: banner
			anchors {
				left: parent.left
				right: parent.right
			}
			height: rootArtist.height * 0.3
			fillMode: Image.PreserveAspectCrop
			clip: true
		}
		Label {
			id: artistName
			anchors {
				left: parent.left
				right: parent.right
				bottom: banner.bottom
			}
			fontSizeMode: Text.Fit
			font.pointSize: 24
			horizontalAlignment: Text.AlignHCenter
			background: Rectangle {
				color: Material.background
				opacity: 0.5
			}
		}

		RowLayout {
			id: infoRow
			anchors {
				left: parent.left
				right: parent.right
				top: banner.bottom
				margins: 16
			}
			Label {
				id: artistInfo
				Layout.fillWidth: true
				wrapMode: Text.Wrap
			}
			Button {
				id: followArtist
				icon.name: "starred-symbolic"
				flat: true
			}
		}

		ListView {
			id: artistList
			anchors {
				left: parent.left
				right: parent.right
				top: infoRow.bottom
				bottom: tabsInfo.top
			}
			model: ListModel {
				id: artistListModel
			}
			delegate: Component {
				Button {
					height: listItemHeight
					width: artistList.width
					flat: true
					onClicked: JS.clickedItem(model.id)
					text: model.text
				}
			}
		}

		TabBar {
			id: tabsInfo
			anchors {
				left: parent.left
				right: parent.right
				bottom: parent.bottom
			}
			currentIndex: 0
			TabButton {
				text: "Popular"
				onClicked: JS.tabChanged()
			}
			TabButton {
				text: "Albums"
				onClicked: JS.tabChanged()
			}
			TabButton {
				text: "Singles"
				onClicked: JS.tabChanged()
			}
			TabButton {
				text: "Related"
				onClicked: JS.tabChanged()
			}
		}
	}
}