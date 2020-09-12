import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Dialogs 1.0

import "js/settingsDialog.js" as JS

Drawer {
	id: root
	width: parent.width
	height: parent.height - toolBar.height
	edge: Qt.BottomEdge

	FileDialog {
		id: spotifydFileDialog
		title: "spotifyd path"
		onAccepted: {
			sptPath.text = fileUrl.toString().replace("file:///", "/")
		}
	}

	onOpened: () => JS.reload()

	ColumnLayout {
		anchors.fill: parent
		StackLayout {
			currentIndex: tabs.currentIndex
			Layout.bottomMargin: drawerTabs.height
			Layout.alignment: Qt.AlignHCenter
			Layout.maximumWidth: parent.width - 64
			Layout.topMargin: 32

			SettingsLayout {
				Label { text: "Refresh interval" }
				ComboBox {
					id: refreshInterval
					Layout.fillWidth: true
					model: ["1", "3", "10"]
				}

				Label { text: "Style" }
				ComboBox {
					id: style
					Layout.fillWidth: true
					model: utils.availableStyles()
				}

				Label { text: "Spotify playback order" }
				Switch {
					id: spotifyPlaybackOrder
				}

				Label { text: "Show what's new on start" }
				Switch {
					id: whatsNewOnStart
				}
			}

			ColumnLayout {
				SettingsLayout {
					Label { text: "spotifyd path" }
					RowLayout {
						TextField {
							id: sptPath
							Layout.fillWidth: true
						}
						Button {
							text: "..."
							flat: true
							onClicked: spotifydFileDialog.open()
						}
					}

					Label { text: "Start with app" }
					Switch {
						id: startWithApp
					}

					Label { text: "Always start" }
					Switch {
						id: alwaysStart
					}

					Label { text: "Global config" }
					Switch {
						id: globalConfig
					}
				}
				GroupBox {
					id: appSettings
					title: "App specific settings"
					Layout.fillWidth: true
					enabled: !globalConfig.checked
					SettingsLayout {
						anchors {
							left: parent.left
							right: parent.right
						}
						Label { text: "Username" }
						TextField {
							id: username
							Layout.fillWidth: true
						}

						Label { text: "Quality" }
						ComboBox {
							id: quality
							Layout.fillWidth: true
							model: ["Low (96 kbit/s)", "Medium (160 kbit/s)", "High (320 kbit/s)"]
						}
					}
				}
			}

			Item {
				Row {
					id: logo
					spacing: 16
					anchors {
						top: parent.top
						horizontalCenter: parent.horizontalCenter
					}
					Image {
						readonly property int size: parent.height
						sourceSize {
							width: size
							height: size
						}
						source: "/res/logo/spotify-qt.svg"
					}
					Column {
						Label {
							text: "spotify-qt-quick"
							font.pointSize: appVersion.font.pointSize * 1.5
						}
						Label {
							id: appVersion
							text: AppVersion
						}
					}
				}
				ColumnLayout {
					anchors {
						left: parent.left
						right: parent.right
						bottom: parent.bottom
						bottomMargin: saveButton.width - 16
					}
					TextArea {
						id: systemInfo
						readOnly: true
						textFormat: TextEdit.RichText
						Layout.fillWidth: true
					}
					Button {
						Layout.fillWidth: true
						text: `About Qt ${QtVersion}`
						onClicked: utils.aboutQt()
					}
					Button {
						Layout.fillWidth: true
						text: "System info"
						onClicked: systemInfo.text = spotify.systemInfo()
					}
				}

			}
		}
		TabBar {
			id: tabs
			Layout.alignment: Qt.AlignBottom
			Layout.fillWidth: true
			currentIndex: 0
			TabButton {
				text: "Application"
			}
			TabButton {
				text: "Spotify"
			}
			TabButton {
				text: "About"
			}
		}
	}

	RoundButton {
		id: saveButton
		readonly property int margin: 16
		readonly property int size: 72
		anchors {
			right: parent.right
			bottom: parent.bottom
			rightMargin: margin
			bottomMargin: margin + tabs.height
		}
		width: size
		height: size
		icon.name: "document-save"
		padding: 24
		onClicked: JS.save()
	}
}