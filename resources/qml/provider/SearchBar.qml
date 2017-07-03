import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import "../global"

Rectangle {
	id: baseItem

	readonly property alias searchText: searchField.text

	property color searchBarColor: Constants.blue

	color: searchBarColor
	height: Constants.searchbar_height

	MouseArea {
		id: pageArea
		onClicked: pageArea.focus = true
		anchors.fill: parent

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

			TextField {
				id: searchField
				anchors.fill: parent
				anchors.leftMargin: Utils.dp(24)

				horizontalAlignment: Text.AlignLeft

				style: TextFieldStyle {
					background: Rectangle {
						radius: Utils.dp(6)
					}
				}

				Image {
					id: textEditX
					anchors.right: parent.right
					anchors.rightMargin: Utils.dp(8)
					anchors.verticalCenter: parent.verticalCenter
					width: Utils.dp(sourceSize.width / 2)
					height: Utils.dp(sourceSize.height / 2)
					source: "qrc:///images/text-edit-x.png"
					visible: searchField.text.length > 0

					MouseArea {
						anchors.fill: parent
						onClicked: {
							searchField.text = ""
						}
					}
				}
			}

			Item {
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.leftMargin: Utils.dp(8)
				anchors.topMargin: Utils.dp(4)

				width: childrenRect.width
				height: glassIcon.height

				Image {
					id: glassIcon
					height: textEditX.height
					width: textEditX.width
					fillMode: Image.PreserveAspectFit
					source: "qrc:///images/magnifying-glass.png"
					visible: searchField.text.trim().length === 0
				}

				Label {
					id: searchLabel
					anchors.left: glassIcon.right
					anchors.leftMargin: Utils.dp(8)
					anchors.verticalCenter: glassIcon.verticalCenter
					text: qsTr("Search")
					color: Constants.grey
					font.pixelSize: Utils.sp(14)
					visible: searchField.text.trim().length === 0
				}
			}
		}

		DimmableTextButton {
			id: cancelButton

			anchors.left: parent.right
			anchors.right: parent.right
			anchors.rightMargin: Utils.dp(8)
			anchors.verticalCenter: parent.verticalCenter
			clip: true
			font.family: "Helvetica"
			font.pixelSize: Utils.sp(15)
			color: "white"
			text: qsTr("Cancel")
			onClicked: {
				 pageArea.clicked(null)
			}
		}
	}

	states: [
		State {
			name: "searchBarActive"
			when: searchField.activeFocus
			AnchorChanges {
				target: cancelButton
				anchors.left: undefined
			}

			PropertyChanges {
				target: cancelButton
				width: cancelButton.implicitWidth
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
				target: searchField
				text: ""
			}

			AnchorChanges {
				target: cancelButton
				anchors.left: parent.right
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
