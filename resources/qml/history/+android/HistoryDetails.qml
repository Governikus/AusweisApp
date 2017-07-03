import QtQuick 2.5

import QtQuick.Controls 1.4

import "../global"

Item {
	id: baseItem

	property string providerAddress: ""

	property int listItemIndex: -1

	property var listModel

	anchors.right: parent.right
	anchors.top: parent.top
	anchors.margins: Utils.dp(5)

	height: parent.height * 0.3
	width: parent.height * 0.3

	Image {
		anchors.centerIn: parent
		height: parent.height
		width: parent.width
		source: "qrc:///images/android/navigation/versionsinformation.png"

		fillMode: Image.PreserveAspectFit
		clip: true
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			popUp.popup()
		}
	}

	Menu {
		id: popUp

		MenuItem {
			text: qsTr("Delete")
			onTriggered: {
				if (typeof(listModel) === "object") {
					listModel.removeRows(listItemIndex, 1)
				}
			}
		}

		MenuItem {
			text: qsTr("Go to online application")
			onTriggered: {
				Qt.openUrlExternally(providerAddress)
			}
		}
	}
}
