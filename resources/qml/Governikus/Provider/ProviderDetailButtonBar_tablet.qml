import QtQuick 2.6

import Governikus.Global 1.0

Item {
	id: baseItem

	height: button.height + Constants.component_spacing
	width: parent.width

	property string selectedCategory: ""
	property string providerIcon: ""
	property string address: ""
	property color titleBarColor

	Image {
		id: icon
		source: baseItem.providerIcon
		asynchronous: true
		height: 2 * baseItem.height
		width: height
		fillMode: Image.PreserveAspectFit
		anchors.left: parent.left
		anchors.leftMargin: Constants.component_spacing
		anchors.verticalCenter: baseItem.top
	}

	GButton {
		id: button
		text: qsTr("ONLINE APPLICATION") + settingsModel.translationTrigger
		buttonColor: baseItem.titleBarColor
		maxWidth: parent.width - icon.width - 3 * Constants.component_spacing
		anchors.left: icon.right
		anchors.leftMargin: Constants.component_spacing
		anchors.bottom: icon.bottom
		enabled: baseItem.address !== ""

		onClicked: {
			if (baseItem.address !== "") {
				Qt.openUrlExternally(baseItem.address)
			}
		}
	}
}
