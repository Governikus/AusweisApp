/*
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	property string address: ""
	property string providerIcon: ""
	property string selectedCategory: ""
	property color titleBarColor

	height: button.height + Constants.component_spacing
	width: parent.width

	Rectangle {
		id: iconContainer
		anchors.left: parent.left
		anchors.leftMargin: Constants.component_spacing
		anchors.verticalCenter: baseItem.top
		border.color: Style.color.border
		border.width: Style.dimens.separator_size
		color: Constants.white
		height: 2 * baseItem.height
		radius: Style.dimens.corner_radius
		width: height

		Image {
			id: icon
			anchors.fill: parent
			anchors.margins: iconContainer.radius / 2
			asynchronous: true
			fillMode: Image.PreserveAspectFit
			source: baseItem.providerIcon
			sourceSize.height: height
		}
	}
	GButton {
		id: button
		anchors.bottom: iconContainer.bottom
		anchors.left: iconContainer.right
		anchors.leftMargin: Constants.component_spacing
		buttonColor: baseItem.titleBarColor
		enabled: baseItem.address !== ""
		icon.source: "qrc:///images/material_open_in_new.svg"

		//: LABEL ANDROID_TABLET IOS_TABLET
		text: qsTr("To provider")
		tintIcon: true

		onClicked: {
			if (baseItem.address !== "") {
				Qt.openUrlExternally(baseItem.address);
			}
		}
	}
}
