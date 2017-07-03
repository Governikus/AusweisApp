import QtQuick 2.7
import QtQuick.Controls 2.0

import "../global"

Item {
	id: baseItem

	property string providerAddress
	property int listItemIndex

	property var listModel

	anchors.right: parent.right
	anchors.top: parent.top
	anchors.margins: Utils.dp(5)

	height: parent.height * 0.3
	width: parent.height * 0.3

	Image {
		anchors.fill: parent
		source: "qrc:///images/android/navigation/versionsinformation.svg"

		fillMode: Image.PreserveAspectFit
		clip: true
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			popUp.open()
		}
	}

	Popup {
		id: popUp
		x: - (width - baseItem.width)
		padding: 0
		focus: true
		modal: true
		dim: false

		onClosed: navBar.close()

		contentItem: Column {
			id: content

			Rectangle {
				color: mouseArea.pressed ? "grey" : "lightgrey"
				height: Math.max(menuText1.height, menuText2.height)
				width: Math.max(menuText1.width, menuText2.width)
				Text {
					id: menuText1
					padding: Utils.dp(10)
					text: qsTr("Delete")
					font.pixelSize: Constants.normal_font_size
				}
				MouseArea {
					id: mouseArea
					anchors.fill: parent
					onClicked: {
						if (typeof(listModel) === "object") {
							listModel.removeRows(listItemIndex, 1)
						}
					}
				}
			}

			Rectangle {
				color: mouseArea2.pressed ? "grey" : "lightgrey"
				height: Math.max(menuText1.height, menuText2.height)
				width: Math.max(menuText1.width, menuText2.width)
				Text {
					id: menuText2
					padding: Utils.dp(10)
					text: qsTr("Go to online application")
					font.pixelSize: Constants.normal_font_size
					onLinkActivated: parent.triggered(link)
				}

				MouseArea {
					id: mouseArea2
					anchors.fill: parent
					onClicked: {
						Qt.openUrlExternally(providerAddress)
					}
				}

			}
		}
	}
}
