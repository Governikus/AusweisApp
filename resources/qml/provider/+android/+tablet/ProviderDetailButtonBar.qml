import QtQuick 2.6

import "../global"

Item {
	id: baseItem

	property string selectedCategory: ""
	property string providerIcon: ""
	property string address: ""
	property color titleBarColor

	readonly property int customWidth: Utils.dp(24)

	Rectangle {
		height: parent.height
		width: parent.width

		anchors.fill: baseItem
		color: Constants.background_color
	}

	Column {
		id: buttonBarContent

		height: parent.height
		width: parent.width - 2 * baseItem.customWidth
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.leftMargin: baseItem.customWidth
		topPadding: -2 * baseItem.customWidth
		spacing: baseItem.customWidth

		Image {
			id: icon
			source: baseItem.providerIcon !== "" ? baseItem.providerIcon :
					Category.buttonImageSource(baseItem.selectedCategory)

			fillMode: Image.PreserveAspectFit
			height: 4 * baseItem.customWidth
			width: height
			anchors.left: parent.left
		}

		Button {
			text: qsTr("ONLINE-APPLICATION")
			buttonColor: baseItem.titleBarColor
			anchors.left: parent.left
			enabled: baseItem.address !== ""

			onClicked: {
				if (baseItem.address !== "") {
					Qt.openUrlExternally(baseItem.address)
				}
			}
		}
	}
}
