import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import "js/settingsDialog.js" as JS

Drawer {
	id: root
	width: parent.width
	height: parent.height - toolBar.height
	edge: Qt.BottomEdge

	GridLayout {
		columns: 2
		rowSpacing: 16
		columnSpacing: 16
		anchors {
			left: parent.left
			right: parent.right
			top: parent.top
			margins: parent.width * 0.1
		}

		Label {
			text: "Style"
		}
		ComboBox {
			id: styleBox
			Layout.fillWidth: true
			model: utils.availableStyles()
		}
	}

	RoundButton {
		anchors {
			right: parent.right
			bottom: parent.bottom
			rightMargin: 16
			bottomMargin: 16
		}
		icon.name: "document-save"
		padding: 24
		onClicked: root.close()
	}
}