import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import Governikus.Global 1.0


Item {
	readonly property alias searchText: searchField.text

	id: baseItem
	height: Constants.searchbar_height

	MouseArea {
		id: pageArea
		onClicked: pageArea.focus = true

		height: Constants.searchbar_height
		width: parent.width
		anchors.bottom: parent.bottom

		Rectangle {
			anchors.left: parent.left
			anchors.right: cancelButton.left
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			anchors.leftMargin: Utils.dp(8)
			anchors.rightMargin: Utils.dp(10)
			anchors.topMargin: Utils.dp(6)
			anchors.bottomMargin: Utils.dp(8)
			radius: Utils.dp(6)
			color: "white"

			Image {
				id: glassIcon
				height: parent.height - 2 * anchors.leftMargin
				width: height
				anchors.left: parent.left
				anchors.leftMargin: Utils.dp(4)
				anchors.verticalCenter: parent.verticalCenter
				source: "qrc:///images/iOS/search_icon.svg"
			}

			TextField {
				id: searchField
				anchors.margins: Utils.dp(8)
				anchors.verticalCenter: parent.verticalCenter
				anchors.left: glassIcon.right
				anchors.right: textEditX.left
				horizontalAlignment: Text.AlignLeft

				style: TextFieldStyle {
					background: Rectangle {}
				}

				onVisibleChanged: {
					if (visible === false){
						Qt.inputMethod.hide()
					}
				}
			}

			Label {
				id: searchLabel
				anchors.left: searchField.left
				anchors.leftMargin: Utils.dp(8)
				anchors.verticalCenter: searchField.verticalCenter
				text: qsTr("Search") + settingsModel.translationTrigger
				color: Constants.grey
				font.pixelSize: Constants.normal_font_size
				visible: searchField.text.trim().length === 0
			}

			MouseArea {
				id: textEditX
				height: parent.height
				width: height
				anchors.right: parent.right
				anchors.verticalCenter: parent.verticalCenter
				enabled: searchField.text.length > 0

				onClicked: {
					searchField.text = ""
				}

				Image {
					anchors.margins: Utils.dp(4)
					anchors.fill: parent
					source: "qrc:///images/iOS/search_cancel.svg"
					visible: parent.enabled
				}
			}
		}

		DimmableTextButton {
			id: cancelButton

			anchors.right: parent.right
			anchors.rightMargin: visible ? Utils.dp(8) : 0
			anchors.verticalCenter: parent.verticalCenter
			clip: true
			visible: false
			width: visible ? cancelButton.implicitWidth : 0
			font.family: "Helvetica"
			font.pixelSize: Constants.normal_font_size
			color: "white"
			text: qsTr("Cancel") + settingsModel.translationTrigger
			onClicked: {
				searchField.text = ""
				pageArea.clicked(null)
			}
		}
	}

	states: [
		State {
			name: "searchBarActive"
			when: searchField.activeFocus || searchField.text.trim().length !== 0

			PropertyChanges {
				target: cancelButton
				visible: true
			}
		},
		State {
			name: ""
			StateChangeScript {
				script: {
					Qt.inputMethod.hide()
				}
			}

			PropertyChanges {
				target: cancelButton
				visible: false
			}
		}
	]

	transitions: [
		Transition {
			from: "*"
			to: "*"
			AnchorAnimation {
				duration: 200;
				easing.type: Easing.InOutQuad
			}
		}
	]
}
