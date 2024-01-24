/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick
import Governikus.Global
import Governikus.Style

Item {
	id: baseItem

	property bool foundSelectedReader: false

	implicitHeight: Style.dimens.workflow_progress_indicator_size
	implicitWidth: phone.implicitWidth

	TintableIcon {
		id: phone

		anchors.centerIn: parent
		source: "qrc:///images/mobile/phone_remote_info.svg"
		sourceSize.height: Style.dimens.header_icon_size
		tintColor: Style.color.control
		visible: !baseItem.foundSelectedReader

		Image {
			id: x

			anchors.horizontalCenter: phone.right
			anchors.horizontalCenterOffset: -phone.width * 0.19
			anchors.verticalCenter: phone.bottom
			anchors.verticalCenterOffset: -phone.height * 0.36
			asynchronous: true
			fillMode: Image.PreserveAspectFit
			source: "qrc:///images/mobile/x.svg"
			sourceSize.height: phone.height * 0.2
		}
	}
	Item {
		id: currentAction

		anchors.bottom: pCircle.top
		anchors.left: parent.left
		anchors.margins: Constants.component_spacing
		anchors.right: parent.right
		anchors.top: parent.top

		CardReader {
			anchors.centerIn: parent
			height: parent.height
			pinFieldAnimation: false
			visible: baseItem.foundSelectedReader
		}
	}
	ProgressCircle {
		id: pCircle

		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		state: baseItem.foundSelectedReader ? "two" : "off"
		visible: baseItem.foundSelectedReader
	}
}
