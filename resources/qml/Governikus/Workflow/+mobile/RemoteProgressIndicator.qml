/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */
import QtQuick 2.15
import Governikus.Global 1.0
import Governikus.Style 1.0

Item {
	id: baseItem

	property bool foundSelectedReader: false

	height: Style.dimens.workflow_progress_indicator_size

	TintableIcon {
		id: phone
		anchors.centerIn: parent
		desaturate: true
		opacity: tintEnabled ? 0.7 : 1.0
		source: "qrc:///images/mobile/phone_remote.svg"
		sourceSize.height: Style.dimens.header_icon_size
		visible: !baseItem.foundSelectedReader
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
