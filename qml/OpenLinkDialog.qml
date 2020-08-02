import com.kraxarn.utils 1.0

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Dialog {
	title: "Link error"
	modal: true
	anchors.centerIn: parent
	standardButtons: Dialog.Ok

	property string linkText

	Utils {
		id: utils
	}

	Timer {
		id: enableCopy
		interval: 3000
		repeat: false
		running: false
		onTriggered: {
			copyButton.text = "Copy to clipboard"
			copyButton.enabled = true
		}
	}

	ColumnLayout {
		anchors {
			left: parent.left
			right: parent.right
		}
		Label {
			wrapMode: Text.WordWrap
			Layout.fillWidth: true
			text: "A default browser could not be found. Please try copying and pasting the following link manually."
		}
		TextArea {
			Layout.fillWidth: true
			text: linkText
		}
		Row {
			Layout.alignment: Qt.AlignRight
			Button {
				id: copyButton
				text: "Copy to clipbarod"
				flat: true
				onClicked: {
					copyButton.text = "Copied to clipboard"
					copyButton.enabled = false
					enableCopy.start()
				}
			}
		}
	}
}